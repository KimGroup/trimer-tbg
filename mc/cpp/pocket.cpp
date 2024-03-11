#include <atomic>
#include <chrono>
#include <complex>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geom.h"

#ifdef TEST
void assert_fail() { std::exit(0); }
#define ASSERT(x)                                                                                                      \
	if (!(x))                                                                                                          \
	{                                                                                                                  \
		std::cout << "assertion failed at " << __LINE__ << std::endl;                                                  \
		assert_fail();                                                                                                 \
	}
#define TEST_ASSERT(x)                                                                                                 \
	if (!(x))                                                                                                          \
	{                                                                                                                  \
		std::cout << "test failed at " << __LINE__ << " " << #x << std::endl;                                          \
		assert_fail();                                                                                                 \
	}
#define TEST_ASSERT2(x, y)                                                                                             \
	if (!(x))                                                                                                          \
	{                                                                                                                  \
		std::cout << "test failed at " << __LINE__ << " " << #x << "; " << (y) << std::endl;                           \
		assert_fail();                                                                                                 \
	}

#else
#define ASSERT(x) (x);
#define TEST_ASSERT(x) (x);
#define TEST_ASSERT2(x, y) (x);
#endif


const double infinity = std::numeric_limits<double>::infinity();
using complex = std::complex<double>;

namespace std
{
template <> struct hash<tuple<int, int, int, int>>
{
	std::size_t operator()(const tuple<int, int, int, int>& x) const
	{
		std::size_t seed = std::hash<int32_t>()(std::get<0>(x));
		seed ^= std::hash<int32_t>()(std::get<1>(x)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(std::get<2>(x)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(std::get<3>(x)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
} // namespace std

struct Options
{
	int domain_length;
	int mono_vacancies = 0;

	bool out_monomono = false;
	bool out_clustercount = false;
	bool out_monodi = false;
	bool out_tritri = false;
	bool out_energy = false;
	bool out_histogram = false;
	bool out_winding_histogram = false;
	bool out_order = false;

	bool idealbrickwall = false;
	bool idealrt3 = false;
	bool metropolis = false;
	bool init_random = false;

	bool multicanonical = false;
	int multicanonical_rounds = 10;
	int multicanonical_round_length = 100000;
	int multicanonical_threads = 1;
	double multicanonical_factor_decay = 0.7;
	double multicanonical_init_factor = 0.03;
	int multicanonical_swap_interval = 5;

	int position_interval = 50000;
	int total_steps = 0;
	int decorr_interval = 1;
	int accumulator_interval = 0;
	int swap_interval = 10;
	int adaptation_interval = 500;
	int adaptation_duration = 500000;

	double j4 = 0;
	double u = infinity;
	std::vector<double> temperatures;
	bool adaptive_pt = false;

	std::string directory;

	double pocket_fraction = 0.75;
};

template<typename Geometry>
struct Sample
{
	using geom_t = Geometry;
	using vertex_t = Geometry::vertex_t;
	using bond_t = Geometry::bond_t;

	struct InteractionCount
	{
		int u = 0;
		int j4 = 0;
		int hard_core = 0;
	};

private:
	std::vector<bond_t> _pocket, _Abar;
	std::unordered_map<bond_t, InteractionCount> _candidates;

public:
	int32_t w, h;
	Geometry geom;

	struct Configuration
	{
		std::vector<bool> bond_occ;
		std::vector<uint8_t> vertex_occ;
		int j4_total, clusters_total;
	};

	Configuration cfg;

	Sample() : w(0), h(0), geom(0, 0)
	{
		cfg.j4_total = 0;
		cfg.clusters_total = 0;
	}

	Sample(int32_t w, int32_t h) : w(w), h(h), geom(w, h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * bond_t::unit_cell_size(), false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h * vertex_t::unit_cell_size(), 0);
		cfg.j4_total = 0;
		cfg.clusters_total = 0;
	}

	Sample(int32_t w, int32_t h, const std::vector<bond_t>& bonds) : Sample(w, h)
	{
		for (auto& i : bonds)
			cfg.bond_occ[i.index(w)] = true;

		cfg.j4_total = -1;
		cfg.clusters_total = -1;

		regenerate_occupation();
		calculate_energy();
	}

	void regenerate_occupation()
	{
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		for (uint i = 0; i < cfg.bond_occ.size(); i++)
			if (cfg.bond_occ[i])
			{
				auto tri = bond_t::from_index(i, w);
				for (auto& vtx : geom.get_vertices(tri))
					cfg.vertex_occ[vtx.index(w)]++;
			}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
	void change_j4_bonds(const bond_t& removed, int amount)
	{
#pragma GCC diagnostic pop
		if constexpr(geom_t::has_j4())
		{
			for (auto& rel : geom.j4_neighbor_list[removed.s])
				if (cfg.bond_occ[geom.principal(rel + removed.lattice_pos()).index(w)])
					cfg.j4_total += amount;
		}
	}

	void calculate_energy()
	{
		if (cfg.clusters_total >= 0 && cfg.j4_total >= 0)
			return;

		cfg.clusters_total = 0;
		cfg.j4_total = 0;

#ifdef TEST
		int total_occ = 0;
		for (const auto& occ : cfg.vertex_occ)
			total_occ += occ;

		for (auto i : cfg.bond_occ)
			if (i)
				total_occ -= (int) std::tuple_size<decltype(geom.get_vertices(bond_t()))>::value;

		ASSERT(total_occ == 0);
#endif

		for (const auto& occ : cfg.vertex_occ)
			cfg.clusters_total += occ * (occ - 1) / 2;

		for (int x = 0; x < w; x++)
			for (int y = 0; y < h; y++)
				if (cfg.bond_occ[bond_t(x, y, 0).index(w)])
					change_j4_bonds(bond_t(x, y, 0), 1);
	}

	void set_pocket_candidates(const bond_t& el, const bond_t& moved, double u, double j4)
	{
		_candidates.clear();

		// hard-core constraint
		if (cfg.bond_occ[moved.index(w)])
			_candidates[moved].hard_core++;

		// add target overlaps to candidates
		if (u != 0)
			for (const auto& i : geom.get_vertices(moved))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates[trimer_pos].u++;

		// add source overlaps to candidates
		if (u != 0 && u < infinity)
			for (const auto& i : geom.get_vertices(el))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates[trimer_pos].u--;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
		if constexpr(geom_t::has_j4())
#pragma GCC diagnostic pop
		{
			if (j4 != 0)
			{
				for (auto& d : geom.j4_neighbor_list[el.s])
				{
					auto pos = geom.principal(d + el.lattice_pos());
					if (cfg.bond_occ[pos.index(w)])
						_candidates[pos].j4--;
				}

				for (auto& d : geom.j4_neighbor_list[moved.s])
				{
					auto pos = geom.principal(d + moved.lattice_pos());
					if (cfg.bond_occ[pos.index(w)])
						_candidates[pos].j4++;
				}
			}
		}
	}

	template<typename Rng> int pocket_move(Rng& rng, double u, double j4)
	{
		calculate_energy();

		LatticePos symc;
		int syma;
		bool symmetry_is_involute = false;

		bond_t seed(0, 0, -1);
		while (seed.s == -1)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());

			symc = LatticePos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % geom.n_symmetries());

			if (cfg.bond_occ[candidate])
			{
				seed = bond_t::from_index(candidate, w);
				if (symmetry_is_involute && cfg.bond_occ[geom.apply_symmetry(seed, symc, syma).index(w)])
					seed = bond_t(0, 0, -1);
			}
		}

		std::uniform_real_distribution<> uniform(0., 1.);

		_Abar.clear();
		_pocket.clear();
		_pocket.push_back(seed);

		cfg.bond_occ[seed.index(w)] = false;
		for (auto& i : geom.get_vertices(seed))
		{
			ASSERT(cfg.vertex_occ[i.index(w)] > 0);
			cfg.vertex_occ[i.index(w)]--;
		}

		int length = 0;
		while (_pocket.size() > 0)
		{
			length++;
			auto el = _pocket.back();
			_pocket.pop_back();

			auto moved = geom.apply_symmetry(el, symc, syma);
			_Abar.push_back(moved);

			set_pocket_candidates(el, moved, u, j4);

			for (auto& [pos, interactions] : _candidates)
			{
				double u_factor = interactions.u != 0 ? u * interactions.u : 0;
				double j4_factor = interactions.j4 != 0 ? j4 * interactions.j4 : 0;

				double p_cascade = 1 - std::exp(-(u_factor + j4_factor));

				if (interactions.hard_core > 0)
					p_cascade = 1;

				if (p_cascade > 0 && (p_cascade >= 1 || uniform(rng) < p_cascade))
				{
					_pocket.push_back(pos);
					cfg.bond_occ[pos.index(w)] = false;
					for (const auto& vtx : geom.get_vertices(pos))
					{
						ASSERT(cfg.vertex_occ[vtx.index(w)] > 0);
						cfg.vertex_occ[vtx.index(w)]--;
					}
				}
			}
		}

		for (auto& i : _Abar)
		{
			ASSERT(cfg.bond_occ[i.index(w)] == false)
			cfg.bond_occ[i.index(w)] = true;

			for (auto& vtx : geom.get_vertices(i))
				cfg.vertex_occ[vtx.index(w)]++;
		}

#ifdef TEST
		auto occupations = cfg.vertex_occ;
		regenerate_occupation();
		ASSERT(occupations == cfg.vertex_occ)
#endif

		// mark dirty
		cfg.j4_total = -1;
		cfg.clusters_total = -1;

		return length;
	}

	template<typename Rng>
	void worm_flip(Rng& rng, vertex_t& pos, bond_t& moving, const bond_t& move_from, int direction)
	{
		ASSERT(cfg.bond_occ[move_from.index(w)]);
		cfg.bond_occ[move_from.index(w)] = false;
		change_j4_bonds(move_from, -1);

		if (moving.s >= 0)
		{
			ASSERT(!cfg.bond_occ[moving.index(w)]);
			cfg.bond_occ[moving.index(w)] = true;
			change_j4_bonds(moving, 1);
		}

		int flipdir = 0;

		constexpr int ndir = std::tuple_size<typename std::remove_reference<decltype(geom.vertex_flips[0])>::type>::value;
		if constexpr (ndir > 1)
			flipdir = (int) (rng() % ndir);

		moving = geom.principal(geom.bond_flips[direction][flipdir] + pos);

		if (cfg.bond_occ[moving.index(w)])
			// hit a hard-core constraint; force reversal
			moving = move_from;
		else
		{
			pos = geom.principal(geom.vertex_flips[direction][flipdir] + pos);
		}
	}

	template<typename Rng> void worm_init(Rng& rng, vertex_t& cur_pos, bond_t& moving, const bond_t& move_from)
	{
		auto vertices = geom.get_vertices(move_from);
		int prev_dir = -1;
		for (int i = 0; i < (int) vertices.size(); i++)
			if (vertices[i] == cur_pos)
				prev_dir = i;

		int dir;
		if (prev_dir == -1)
			dir = (int) (rng() % vertices.size());
		else
			dir = (int) (((rng() % (vertices.size() - 1)) + prev_dir) % vertices.size());
		
		// start the chain and remove a charge
		cur_pos = vertices[dir];
		cfg.vertex_occ[cur_pos.index(w)]--;
		cfg.clusters_total -= cfg.vertex_occ[cur_pos.index(w)];

		// set the correct direction for the flip based on which vertex was chosen
		worm_flip(rng, cur_pos, moving, move_from, dir * 2 + move_from.s);
	}

	template<typename Rng> int worm_move(Rng& rng, double u)
	{
		calculate_energy();

		bond_t seed(0, 0, -1);
		while (seed.s == -1)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (cfg.bond_occ[candidate])
				seed = bond_t::from_index(candidate, w);
		}

		// init to invalid value so that worm_init does not exclude any direction
		vertex_t pos(-1, -1);

		// holds the temporary position of the moving trimer
		bond_t moving(0, 0, -1);

		worm_init(rng, pos, moving, seed);

		std::uniform_real_distribution<> uniform(0., 1.);
		int length = 0;

		std::vector<int> found_directions;

		while (true)
		{
			length++;

			ASSERT(!cfg.bond_occ[moving.index(w)]);

			int occupation = cfg.vertex_occ[pos.index(w)];
			double p_continue = 1 - std::exp(-u * occupation);
			if (occupation < 1 || uniform(rng) > p_continue)
				break;
			
			found_directions.clear();
			auto trimers = geom.get_bonds(pos);

			for (int i = 0; i < (int) trimers.size(); i++)
				if (cfg.bond_occ[trimers[i].index(w)])
					found_directions.push_back(i);

			ASSERT(found_directions.size() == (size_t) occupation);

			int new_dir = found_directions[rng() % found_directions.size()];
			const auto& chosen_trimer = trimers[new_dir];

			worm_flip(rng, pos, moving, chosen_trimer, new_dir);
		}

		ASSERT(!cfg.bond_occ[moving.index(w)]);
		cfg.bond_occ[moving.index(w)] = true;
		change_j4_bonds(moving, 1);

		cfg.clusters_total += cfg.vertex_occ[pos.index(w)];
		cfg.vertex_occ[pos.index(w)]++;


#ifdef TEST
		int oldj4 = cfg.j4_total;
		int oldcluster = cfg.clusters_total;
		cfg.j4_total = -1;
		cfg.clusters_total = -1;

		calculate_energy();
		ASSERT(cfg.j4_total == oldj4)
		ASSERT(cfg.clusters_total == oldcluster)

		auto occupations = cfg.vertex_occ;
		regenerate_occupation();
		ASSERT(occupations == cfg.vertex_occ)
#endif

		return length;
	}

	template <typename Rng> void metropolis_move(Rng& rng)
	{
		bond_t seed;
		while (true)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (cfg.bond_occ[candidate])
			{
				seed = bond_t::from_index(candidate, w);
				cfg.bond_occ[candidate] = false;
				break;
			}
		}
		bond_t dest;
		while (true)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (!cfg.bond_occ[candidate])
			{
				dest = bond_t::from_index(candidate, w);
				break;
			}
		}

		change_j4_bonds(seed, -1);
		change_j4_bonds(dest, 1);

		int dcluster = 0;
		for (const auto& vtx : geom.get_vertices(seed))
		{
			auto& count = cfg.vertex_occ[vtx.index(w)];
			// difference due to removing one
			// (x-1)(x-2)/2 - x(x-1)/2 = 1 - x
			dcluster += 1 - count;
			ASSERT(count > 0);
			count--;
		}
		for (const auto& vtx : geom.get_vertices(dest))
		{
			auto& count = cfg.vertex_occ[vtx.index(w)];
			// difference due to adding one
			// x(x+1)/2 - x(x-1)/2 = x
			dcluster += count;
			count++;
		}

		cfg.bond_occ[dest.index(w)] = true;
		cfg.clusters_total += dcluster;

#ifdef TEST
		int oldj4 = cfg.j4_total;
		int oldcluster = cfg.clusters_total;
		cfg.j4_total = -1;
		cfg.clusters_total = -1;
		calculate_energy();
		ASSERT(cfg.j4_total == oldj4)
		ASSERT(cfg.clusters_total == oldcluster)

		auto occupations = cfg.vertex_occ;
		regenerate_occupation();
		ASSERT(occupations == cfg.vertex_occ)
#endif
	}
};

template<typename T> struct Accumulator
{
	std::vector<T> mean;
	std::vector<T> m2;

	int interval;
	int total = 0;

	Accumulator() : interval(1) {}
	Accumulator(uint n, int interval) : mean(n), m2(n), interval(interval) {}

	void record(const std::vector<T>& vals)
	{
		total++;

		double inv_total = (1. / total);

		for (uint i = 0; i < vals.size(); i++)
		{
			double delta = vals[i] - mean[i];
			mean[i] += delta * inv_total;
			double delta2 = vals[i] - mean[i];
			m2[i] += delta * delta2;
		}
	}

	void write(std::ostream& os, bool force = false)
	{
		if (total > 0 && (total >= interval || force))
		{
			os << total << std::endl;
			for (auto i : mean)
				os << i << " ";
			os << std::endl;
			for (auto i : m2)
				os << i << " ";
			os << std::endl;

			reset();
		}
	}

	void reset()
	{
		total = 0;
		std::fill(mean.begin(), mean.end(), 0);
		std::fill(m2.begin(), m2.end(), 0);
	}
};

template <typename T> struct MovingAverage
{
	std::vector<T> mean;
	double t;

	MovingAverage() {}

	MovingAverage(int n, double t) : mean(n, 0. / 0.), t(t) {}

	void record(const std::vector<T>& vals)
	{
		double decay = std::exp(-1 / t);
		for (uint i = 0; i < mean.size(); i++)
		{
			if (mean[i] != mean[i])
				mean[i] = vals[i];
			else
				mean[i] = mean[i] * decay + (1 - decay) * vals[i];
		}
	}
};

template<typename T> struct Observer { };

template<>
struct Observer<DimerSquareGeometry>
{
	using sample_t = Sample<DimerSquareGeometry>;
	using bond_t = sample_t::bond_t;
	using vertex_t = sample_t::vertex_t;

	int calculate_winding_number(const sample_t& sample, int dir) const
	{
		bond_t start(0, 0, 0);

        if (dir == 0)
            start.s = 1;
        else
            start.s = 0;

        auto pos = start;
		int tot = 0;
		int sign = 1;

		while (true)
		{
			if (sample.cfg.bond_occ[pos.index(sample.w)])
				tot += sign;

            if (dir == 0)
                pos.x++;
            else
                pos.y++;
			pos = sample.geom.principal(pos);

			sign *= -1;
			if (pos == start)
				break;
		}
		return tot;
	}

	std::pair<int, int> calculate_topo_sector(const sample_t& sample) const
	{
        int a = calculate_winding_number(sample, 0);
        int b = calculate_winding_number(sample, 1);

		return std::make_pair(a, b);
	}
};

template<>
struct Observer<TrimerTriangularGeometry>
{
	using sample_t = Sample<TrimerTriangularGeometry>;
	using vertex_t = sample_t::vertex_t;
	using bond_t = sample_t::bond_t;

	std::unordered_map<int, std::vector<complex>> _ft_contributions;

	template<typename Rng>
	void record_dimer_monomer_correlations(const sample_t& sample, Accumulator<double>& a, Rng& rng, int max_dimers) const
	{
		std::vector<double> vals(sample.w * sample.h);

		std::vector<std::pair<vertex_t, int>> dimers;
		std::unordered_set<vertex_t> isolated_monos;
		std::unordered_set<vertex_t> connected_monos;

		const std::array<vertex_t, 3> neighbors = {vertex_t{1, 0}, {0, 1}, {-1, 1}};

		for (uint i = 0; i < sample.cfg.vertex_occ.size(); i++)
		{
			if (sample.cfg.vertex_occ[i] > 0)
				continue;

			bool connected = false;
			auto pos = vertex_t::from_index(i, sample.w);
			for (int r = 0; r < 3; r++)
			{
				auto neighbor = sample.geom.principal(pos + neighbors[r]);
				if (sample.cfg.vertex_occ[neighbor.index(sample.w)] == 0)
				{
					dimers.push_back(std::make_pair(pos, r));
					isolated_monos.erase(neighbor);
					connected_monos.insert(neighbor);
					connected = true;
				}
			}

			if (connected)
				connected_monos.insert(pos);
			else if (!connected_monos.contains(pos))
				isolated_monos.insert(pos);
		}

		std::shuffle(dimers.begin(), dimers.end(), rng);

		for (uint i = 0; i < std::min((size_t)max_dimers, dimers.size()); i++)
		{
			auto dimer = dimers[i];
			for (auto j : isolated_monos)
				vals[sample.geom.principal(sample.geom.rotate(sample.geom.center(j, dimer.first), -dimer.second)).index(sample.w)]++;
		}

		a.record(vals);
	}

	double trimer_correlation(const sample_t& sample, const bond_t& d) const
	{
		int total = 0;
		int found = 0;

		for (uint i = 0; i < sample.cfg.bond_occ.size(); i++)
		{
			if (sample.cfg.bond_occ[i])
			{
				bond_t p1 = bond_t::from_index(i, sample.w);
				bond_t n;

				if (p1.s == 1)
					n = bond_t(-p1.x + d.x, -p1.y + d.y, (int8_t) (1 - d.s));
				else
					n = bond_t(p1.x + d.x, p1.y + d.y, d.s);

				if (sample.cfg.bond_occ[sample.geom.principal(n).index(sample.w)])
					found++;
				total++;
			}
		}

		return (double)found / total;
	}

	void record_trimer_correlations(const sample_t& sample, Accumulator<double>& a) const
	{
		std::vector<double> vals(sample.w * sample.h * 2);

		for (uint i = 0; i < sample.cfg.bond_occ.size(); i++)
		{
			if (sample.cfg.bond_occ[i])
				for (uint j = i + 1; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
					{
						auto p1 = bond_t::from_index(i, sample.w);
						auto p2 = bond_t::from_index(j, sample.w);

						auto d = p1.s == 0 ? bond_t(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : bond_t(p1.x - p2.x, p1.y - p2.y, (int8_t) (1 - p2.s));

						vals[sample.geom.principal(d).index(sample.w)]++;
					}
		}

		a.record(vals);
	}

	template<typename Rng>
	void record_partial_trimer_correlations(const sample_t& sample, Accumulator<double>& a, Rng& rng, double fraction) const
	{
		std::vector<double> vals(sample.w * sample.h * 2);
		std::uniform_real_distribution<> uniform(0., 1.);

		int seen = 0;
		for (uint i = 0; i < sample.cfg.bond_occ.size(); i++)
		{
			if (sample.cfg.bond_occ[i] && uniform(rng) <= fraction)
			{
				seen++;
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
					{
						if (j == i)
							continue;

						auto p1 = bond_t::from_index(i, sample.w);
						auto p2 = bond_t::from_index(j, sample.w);
						if (j > i)
							std::swap(p1, p2);

						auto d = p1.s == 0 ? bond_t(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : bond_t(p1.x - p2.x, p1.y - p2.y, (int8_t) (1 - p2.s));

						vals[sample.geom.principal(d).index(sample.w)]++;
					}
			}
		}

		if (seen > 0)
		{
			double inv_seen = 1. / seen;
			for (auto& n : vals)
				n *= inv_seen;
		}

		a.record(vals);
	}

	struct Observables
	{
		double structure_factor_K;
		double structure_factor_K1;
		double structure_factor_K2;
		int sublattice_polarization;
	};

	Observables record_order_parameters(const sample_t& sample, Accumulator<double>& acc)
	{
		std::vector<double> vals(11);

		const double rt3 = std::sqrt(3.);
		const double Kpx0 = 4. / 3, Kpy0 = 0;
		// const double Kpx1 = -2. / 3, Kpy1 = 2 * rt3 / 3;
		// const double Kpx2 = -2. / 3, Kpy2 = -2 * rt3 / 3;
		const double Kpx1 = 4. / 3, Kpy1 = 4. / rt3 / (double) sample.h;
		const double Kpx2 = 4. / 3, Kpy2 = 8. / rt3 / (double) sample.w;

		const double Mpx0 = 0, Mpy0 = 2 * rt3 / 3;
		const double Mpx1 = 1, Mpy1 = -rt3 / 3;
		const double Mpx2 = -1, Mpy2 = -rt3 / 3;

		complex M0, M1, M2, K0, K1, K2;

		int AmB = 0;

		for (int i = 0; i < (int) sample.cfg.bond_occ.size(); i++)
		{
			if (sample.cfg.bond_occ[i])
			{
				auto pos = bond_t::from_index(i, sample.w);
				if (!_ft_contributions.contains(i))
				{
					double x = pos.x + pos.y * 0.5;
					double y = pos.y * rt3 / 2;
					if (pos.s == 1)
					{
						x += 0.5;
						y += 2 / rt3;
					}

					double dot = (x * Mpx0 + y * Mpy0) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));
					dot = (x * Mpx1 + y * Mpy1) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));
					dot = (x * Mpx2 + y * Mpy2) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));

					dot = (x * Kpx0 + y * Kpy0) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));
					dot = (x * Kpx1 + y * Kpy1) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));
					dot = (x * Kpx2 + y * Kpy2) * M_PI;
					_ft_contributions[i].push_back(complex(std::cos(dot), std::sin(dot)));
				}

				auto& cont = _ft_contributions[i];
				M0 += cont[0];
				M1 += cont[1];
				M2 += cont[2];
				K0 += cont[3];
				K1 += cont[4];
				K2 += cont[5];

				AmB += pos.s * 2 - 1;
			}
		}

		vals[0] = (std::abs(M0) + std::abs(M1) + std::abs(M2)) / 3.;
		vals[1] = vals[0] * vals[0];
		vals[2] = vals[1] * vals[1];

		vals[3] = std::abs(K0);
		vals[4] = vals[3] * vals[3];
		vals[5] = vals[4] * vals[4];

		vals[6] = std::abs(K1);
		vals[7] = std::abs(K2);

		vals[8] = std::abs(AmB);
		vals[9] = vals[8] * vals[8];
		vals[10] = vals[9] * vals[9];

		acc.record(vals);

		return Observables{std::abs(K0), std::abs(K1), std::abs(K2), AmB};
	}

	template <typename Rng>
	void record_partial_monomer_correlations(const sample_t& sample, Accumulator<double>& a, Rng& rng, int max_count) const
	{
		std::vector<double> vals(sample.w * sample.h);
		std::vector<vertex_t> pos;

		for (uint i = 0; i < sample.cfg.vertex_occ.size(); i++)
			if (sample.cfg.vertex_occ[i] == 0)
				pos.push_back(vertex_t::from_index(i, sample.w));

		std::shuffle(pos.begin(), pos.end(), rng);
		uint sample_count = (uint)std::min(pos.size(), (size_t)max_count);

		for (uint i = 0; i < sample_count; i++)
		{
			for (uint j = 0; j < pos.size(); j++)
			{
				if (j == i)
					continue;

				auto d = pos[i] - pos[j];
				vals[sample.geom.principal(d).index(sample.w)] += 0.5;
				vals[sample.geom.principal(-d).index(sample.w)] += 0.5;
			}
		}

		double inv_count = 1. / sample_count;
		for (auto& x : vals)
			x *= inv_count;

		a.record(vals);
	}

	void record_monomer_correlations(const sample_t& sample, Accumulator<double>& a) const
	{
		std::vector<double> vals(sample.w * sample.h);
		std::vector<vertex_t> pos;

		for (uint i = 0; i < sample.cfg.vertex_occ.size(); i++)
			if (sample.cfg.vertex_occ[i] == 0)
				pos.push_back(vertex_t::from_index(i, sample.w));

		for (uint i = 0; i < pos.size(); i++)
		{
			for (uint j = i + 1; j < pos.size(); j++)
			{
				auto d = pos[i] - pos[j];
				vals[sample.geom.principal(d).index(sample.w)] += 1;
				vals[sample.geom.principal(-d).index(sample.w)] += 1;
			}
		}

		double inv_count = 1. / (double) pos.size();
		for (auto& x : vals)
			x *= inv_count;

		a.record(vals);
	}

	void record_cluster_count(const sample_t& sample, Accumulator<double>& a) const
	{
		std::vector<double> vals(7);

		for (uint i = 0; i < sample.cfg.vertex_occ.size(); i++)
			vals[sample.cfg.vertex_occ[i]]++;

		a.record(vals);
	}

	double record_energy(sample_t& sample, Accumulator<double>& a, double u, double j4) const
	{
		std::vector<double> vals(5);
		sample.calculate_energy();

		vals[0] = sample.cfg.clusters_total;
		vals[1] = sample.cfg.j4_total;

		if (u < infinity)
			vals[2] = sample.cfg.clusters_total * u + sample.cfg.j4_total * j4;
		else
			vals[2] = sample.cfg.j4_total * j4;

		vals[3] = vals[2] * vals[2];
		vals[4] = vals[3] * vals[3];

		a.record(vals);

		return vals[2];
	}

	int calculate_winding_number(const sample_t& sample, bond_t start, int dir) const
	{
		bond_t cpos = start;
		int tot = -(sample.w + sample.h) / 6;

		while (true)
		{
			if (sample.cfg.bond_occ[cpos.index(sample.w)])
				tot++;

			switch (dir % 3)
			{
			case 0:
				if (cpos.s == 0)
					cpos = bond_t(cpos.x, cpos.y, 1);
				else
					cpos = bond_t(cpos.x + 1, cpos.y + 1, 0);
				break;
			case 1:
				if (cpos.s == 0)
					cpos = bond_t(cpos.x - 1, cpos.y, 1);
				else
					cpos = bond_t(cpos.x - 1, cpos.y + 1, 0);
				break;
			case 2:
				if (cpos.s == 0)
					cpos = bond_t(cpos.x, cpos.y - 1, 1);
				else
					cpos = bond_t(cpos.x + 1, cpos.y - 1, 0);
				break;
			}

			cpos = sample.geom.principal(cpos);

			if (cpos == start)
				break;
		}
		return tot;
	}

	std::tuple<int, int, int, int> calculate_topo_sector(const sample_t& sample) const
	{
		int wa = calculate_winding_number(sample, bond_t(0, 0, 0), 0);
		int wb = calculate_winding_number(sample, bond_t(2, 0, 0), 1);
		int wc = calculate_winding_number(sample, bond_t(1, 0, 0), 0);
		int wd = calculate_winding_number(sample, bond_t(0, 0, 0), 1);

		int ri = (wa - wb) / 3, rj = (2 * wa + wb) / 3;
		int gi = (wc - wd) / 3, gj = (2 * wc + wd) / 3;
		return std::make_tuple(ri, rj, gi, gj);
	}
};

template<>
struct Observer<DimerHexagonalSkewGeometry>
{
	using sample_t = Sample<DimerHexagonalSkewGeometry>;
	using vertex_t = sample_t::vertex_t;
	using bond_t = sample_t::bond_t;

	int calculate_winding_number(const sample_t& sample, int dir) const
	{
		bond_t start(0, 0, 0);
		if (dir == 0)
			start.s = 2;
		else
			start.s = 1;

        auto pos = start;
		int bonds = 0;
        int total = 0;

		while (true)
		{
            total++;
			if (sample.cfg.bond_occ[pos.index(sample.w)])
				bonds++;

			if (dir == 0)
				pos.x++;
			else
				pos.y++;

			pos = sample.geom.principal(pos);

			if (pos == start)
				break;
		}

		return bonds - total / 3;
	}

	std::pair<int, int> calculate_topo_sector(const sample_t& sample) const
	{
		int wa = calculate_winding_number(sample, 0);
		int wb = calculate_winding_number(sample, 1);

		// int ri = (wa - wb) / 3, rj = (2 * wa + wb) / 3;
		return std::make_pair(wa, wb);
	}
};

template<typename T> struct SampleInitializer { };

template<>
struct SampleInitializer<TrimerTriangularGeometry>
{
	using sample_t = Sample<TrimerTriangularGeometry>;

	template<typename Rng> void initialize(sample_t& sample, const Options& opt, Rng& rng)
	{
		if (opt.init_random)
		{
			std::ostringstream ss;
			ss << "new-data/disordered/" << opt.domain_length << "x" << opt.domain_length
			   << "_r-0_t0.000000_j0.000_20000000.200_0/positions.dat";
			load_from(sample, ss.str(), rng, opt.mono_vacancies / 3);
		}
		else
		{
			if (opt.j4 < 0)
				reconfigure_brick_wall(sample, rng, opt.mono_vacancies / 3);
			else
				reconfigure_root3(sample, rng, opt.mono_vacancies / 3);
		}
	}

	template<typename Rng> void reconfigure_brick_wall(sample_t& sample, Rng& rng, int vacancies = 0)
	{
		int w = sample.w, h = sample.h;

		if (w != h || w % 6)
			throw "";

		std::fill(sample.cfg.bond_occ.begin(), sample.cfg.bond_occ.end(), false);
		std::fill(sample.cfg.vertex_occ.begin(), sample.cfg.vertex_occ.end(), 0);

		int orientation = (int)(rng() % 3);
		int phase = (int)(rng() % 3);

		std::vector<int> phase_conf;
		phase_conf.push_back(phase);

		std::uniform_real_distribution<> uniform(0., 1.);

		for (int i = 1; i < h / 2; i++)
		{
			if (phase_conf[i - 1] == phase_conf[0])
				phase_conf.push_back((rng() % 2) ? (phase_conf[0] + 1) % 3 : (phase_conf[0] + 2) % 3);
			else
			{
				int n = h / 2 - i + 1;
				int sign = (n % 2 == 0) ? 1 : -1;
				double p_return = (std::pow(2, n - 1) - 2 * sign) / (std::pow(2, n) - sign);

				phase_conf.push_back(uniform(rng) <= p_return ? phase_conf[0] : 3 - phase_conf[0] - phase_conf[i - 1]);
			}
		}

		sample_t::bond_t cur_row_start = sample_t::bond_t(0, 0, 0);

		// offset row
		if (rng() % 2)
		{
			if (orientation == 2)
				cur_row_start = sample_t::bond_t(1, 0, 0);
			else
				cur_row_start = sample_t::bond_t(0, 1, 0);
		}

		int n = 0;
		for (int row = 0; row < h / 2; row++)
		{
			uint offset = phase_conf[row] + row + 3 * (int)(rng() % w);

			for (int col = 0; col < 2 * w / 3; col++)
			{
				sample_t::bond_t cur_pos;
				switch (orientation)
				{
				case 0:
					cur_pos = sample_t::bond_t(cur_row_start.x + offset / 2, cur_row_start.y, offset % 2);
					break;
				case 1:
					cur_pos =
						sample_t::bond_t(cur_row_start.x - offset / 2 - offset % 2, cur_row_start.y + offset / 2, offset % 2);
					break;
				case 2:
					cur_pos = sample_t::bond_t(cur_row_start.x, cur_row_start.y - (offset + 1) / 2, offset % 2);
					break;
				}

				offset += 3;

				if (n++ >= vacancies)
				{
					sample.cfg.bond_occ[sample.geom.principal(cur_pos).index(w)] = true;
					for (auto& vtx : sample.geom.get_vertices(cur_pos))
						sample.cfg.vertex_occ[vtx.index(w)]++;
				}
			}

			switch (orientation)
			{
			case 0:
				cur_row_start = sample_t::bond_t(cur_row_start.x, cur_row_start.y + 2, 0);
				break;
			case 1:
				cur_row_start = sample_t::bond_t(cur_row_start.x - 2, cur_row_start.y, 0);
				break;
			case 2:
				cur_row_start = sample_t::bond_t(cur_row_start.x + 2, cur_row_start.y - 2, 0);
				break;
			}
		}

		sample.cfg.j4_total = -1;
		sample.cfg.clusters_total = 0;
	}

	template <typename Rng> void load_from(sample_t& sample, std::string file, Rng& rng, int vacancies, int line_number = -1)
	{
		std::fill(sample.cfg.bond_occ.begin(), sample.cfg.bond_occ.end(), false);
		std::fill(sample.cfg.vertex_occ.begin(), sample.cfg.vertex_occ.end(), 0);

		std::ifstream ifs(file);
		if (ifs.fail())
			throw "random file not found";

		std::string line;
		std::vector<std::string> lines;
		std::vector<sample_t::bond_t> positions;
		while (std::getline(ifs, line))
			lines.push_back(line);

		int randline = (int)(rng() % (2 * lines.size() / 3)) + (int)lines.size() / 3;
		if (line_number >= 0)
			randline = line_number;

		line = lines[randline];
		std::istringstream ss(line);
		std::string token;
		while (std::getline(ss, token, ' '))
		{
			std::istringstream ss2(token.substr(1, token.length() - 2));
			std::string token2;
			std::vector<int> nums;
			while (std::getline(ss2, token2, ','))
				nums.push_back(std::stoi(token2));

			positions.push_back(sample_t::bond_t(nums[0], nums[1], (int8_t) nums[2]));
		}

		if (positions.size() != (uint)(sample.w * sample.h / 3))
			throw "file loading failed";

		std::shuffle(positions.begin(), positions.end(), rng);
		for (uint i = vacancies; i < positions.size(); i++)
		{
			const auto& pos = positions[i];
			sample.cfg.bond_occ[pos.index(sample.w)] = true;
			for (auto& vtx : sample.geom.get_vertices(pos))
				sample.cfg.vertex_occ[vtx.index(sample.w)]++;
		}

		sample.cfg.j4_total = -1;
		sample.cfg.clusters_total = -1;
	}

	template<typename Rng> void reconfigure_root3(sample_t& sample, Rng& rng, int vacancies = 0)
	{
		if (sample.w % 3 || sample.h % 3)
			throw "";

		std::fill(sample.cfg.bond_occ.begin(), sample.cfg.bond_occ.end(), false);
		std::fill(sample.cfg.vertex_occ.begin(), sample.cfg.vertex_occ.end(), 0);

		int offset = (int) (rng() % 6);

		int n = 0;
		for (int i = 0; i < sample.w; i += 3)
			for (int j = 0; j < sample.h; j += 3)
				for (int s = 0; s < 3; s++)
					if (n++ >= vacancies)
					{
						sample_t::bond_t pos;
						pos = sample_t::bond_t(i + offset / 2 + s, j + s, (int8_t) (offset % 2));
						sample.cfg.bond_occ[sample.geom.principal(pos).index(sample.w)] = true;
						for (auto& vtx : sample.geom.get_vertices(pos))
							sample.cfg.vertex_occ[vtx.index(sample.w)]++;
					}

		sample.cfg.j4_total = 0;
		sample.cfg.clusters_total = 0;
	}
};

template<typename T> inline void write_binary(std::ostream& o, const T& t)
{
	o.write(reinterpret_cast<const char*>(&t), sizeof(t));
}

struct PTWorker
{
	using Geometry = TrimerTriangularGeometry;

	const Options options;
	double temperature;

	Sample<Geometry> sample;
	Observer<Geometry> obs;
	SampleInitializer<Geometry> init;

	std::minstd_rand rng;

	bool io_initialized = false;
	std::ofstream ofmonomono;
	std::ofstream ofclustercount;
	std::ofstream ofmonodi;
	std::ofstream oftritri;
	std::ofstream oforder;
	std::ofstream ofenergy;
	std::ofstream ofhistogram;
	std::ofstream ofsector;
	std::ofstream ofconf;

	Accumulator<double> amonomono;
	Accumulator<double> aclustercount;
	Accumulator<double> amonodi;
	Accumulator<double> aorder;
	Accumulator<double> atritri;
	Accumulator<double> aenergy;

	Accumulator<double> timing;

	int total_steps = 0;
	int previous_position_output = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

#ifdef ERGOTEST
	std::ofstream ofunique;
	std::ofstream oftrimerhist;
	std::ofstream oftopohist;
	std::unordered_set<std::vector<bool>> seen;
#endif

	PTWorker() : options(Options()) {}

	PTWorker(const Options& opt, double temperature, bool output) : options(opt), temperature(temperature), timing(2, 1)
	{
		start_time = std::chrono::high_resolution_clock::now();
		sample = Sample<Geometry>(opt.domain_length, opt.domain_length);

		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		init.initialize(sample, opt, rng);

		if (output)
			enable_io();

#ifdef ERGOTEST
		std::stringstream fname;
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << options.pocket_fraction << ".csv";
		ofunique = std::ofstream(fname.str());
		fname.str("");
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << options.pocket_fraction << "_thist.dat";
		oftrimerhist = std::ofstream(fname.str(), std::ios::binary);
		fname.str("");
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << options.pocket_fraction << "_topohist.dat";
		oftopohist = std::ofstream(fname.str(), std::ios::binary);
#endif
	}

	void enable_io()
	{
#ifdef ERGOTEST
		return;
#endif

		if (!io_initialized)
		{
			io_initialized = true;
			if (options.directory.empty())
				return;

			std::filesystem::path basepath;
			int runid = 0;
			do
			{
				std::stringstream ss;
				if (options.u == 1)
				{
					ss << options.domain_length << "x" << options.domain_length << "_r-" << options.mono_vacancies
					   << "_t" << std::fixed << std::setprecision(6) << temperature << "_j" << std::setprecision(3)
					   << options.j4 << "_" << options.total_steps << "." << options.decorr_interval << "_" << runid++;
				}
				else
				{
					ss << options.domain_length << "x" << options.domain_length << "_r-" << options.mono_vacancies
					   << "_t" << std::fixed << std::setprecision(6) << temperature << "_u" << std::setprecision(3)
					   << options.u << "_j" << std::setprecision(3) << options.j4 << "_" << options.total_steps << "."
					   << options.decorr_interval << "_" << runid++;
				}

				basepath = std::filesystem::path("new-data") / options.directory / ss.str();
			} while (std::filesystem::exists(basepath));
			std::filesystem::create_directories(basepath);
			std::cout << "saving to " << basepath << std::endl;

			ofmonomono = std::ofstream(basepath / "mono-mono.dat");
			ofclustercount = std::ofstream(basepath / "cluster-count.dat");
			ofmonodi = std::ofstream(basepath / "mono-di.dat");
			oftritri = std::ofstream(basepath / "tri-tri.dat");
			oforder = std::ofstream(basepath / "order.dat");
			ofenergy = std::ofstream(basepath / "energy.dat");
			ofhistogram = std::ofstream(basepath / "histogram.dat", std::ios::binary);
			ofsector = std::ofstream(basepath / "winding-histogram.dat", std::ios::binary);
			ofconf = std::ofstream(basepath / "positions.dat");

			amonomono =
				Accumulator<double>(options.domain_length * options.domain_length, options.accumulator_interval);
			aorder = Accumulator<double>(11, options.accumulator_interval);
			aclustercount = Accumulator<double>(7, options.accumulator_interval);
			amonodi = Accumulator<double>(options.domain_length * options.domain_length, options.accumulator_interval);
			atritri =
				Accumulator<double>(options.domain_length * options.domain_length * 2, options.accumulator_interval);
			aenergy = Accumulator<double>(5, options.accumulator_interval);
		}
	}

	void step()
	{
		auto begin = std::chrono::high_resolution_clock::now();

		double j4 = temperature == 0 ? 0 : options.j4 / temperature;
		double u = temperature == 0 ? infinity : options.u / temperature;

		std::uniform_real_distribution<> uniform(0., 1.);
		for (int i = 0; i < options.decorr_interval; i++)
		{
			if (options.idealbrickwall)
				init.reconfigure_brick_wall(sample, rng, false);
			else if (options.idealrt3)
				init.reconfigure_root3(sample, rng);
			else if (options.metropolis)
			{
				sample.calculate_energy();
				Sample<Geometry>::Configuration copy = sample.cfg;
				sample.metropolis_move(rng);
				sample.calculate_energy();

				double d_energy =
					(sample.cfg.j4_total - copy.j4_total) * j4 + (sample.cfg.clusters_total - copy.clusters_total) * u;
				if (uniform(rng) <= 1 - std::exp(-d_energy))
					std::swap(copy, sample.cfg);
			}
			else
			{
				if (uniform(rng) < options.pocket_fraction)
					sample.pocket_move(rng, u, j4);
				else if (j4 == 0)
					sample.worm_move(rng, u);
				else
				{
					sample.calculate_energy();
					Sample<Geometry>::Configuration copy = sample.cfg;
					sample.worm_move(rng, u);
					sample.calculate_energy();

					double d_energy = (sample.cfg.j4_total - copy.j4_total) * j4;
					if (uniform(rng) <= 1 - std::exp(-d_energy))
						std::swap(copy, sample.cfg);
				}
			}

#ifdef ERGOTEST
			if (sample.w <= 15)
				seen.insert(sample.cfg.bond_occ);
			write_binary<float>(oftrimerhist, (float)obs.trimer_correlation(sample, sample_t::bond_t(1, 0, 1)));

			auto sector = obs.calculate_topo_sector(sample);
			write_binary<int16_t>(oftopohist, (int16_t)std::get<0>(sector));
			write_binary<int16_t>(oftopohist, (int16_t)std::get<1>(sector));
			write_binary<int16_t>(oftopohist, (int16_t)std::get<2>(sector));
			write_binary<int16_t>(oftopohist, (int16_t)std::get<3>(sector));
#endif
		}

		total_steps += options.decorr_interval;
		sample.calculate_energy();

		auto end1 = std::chrono::high_resolution_clock::now();
		std::vector<double> timings(2);
		timings[0] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();

#ifdef ERGOTEST
		double totaltime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start_time).count();
		ofunique << totaltime / 1000 << "," << total_steps << "," << seen.size() << std::endl;
#endif

		if (io_initialized)
		{
			Observer<Geometry>::Observables ordervals;
			double energy = 0;

			if (options.out_monomono)
				obs.record_partial_monomer_correlations(sample, amonomono, rng, 32);
			if (options.out_clustercount)
				obs.record_cluster_count(sample, aclustercount);
			if (options.out_monodi)
				obs.record_dimer_monomer_correlations(sample, amonodi, rng, 10);
			if (options.out_tritri)
				obs.record_partial_trimer_correlations(sample, atritri, rng, 20. / (sample.w * sample.h / 3));
			if (options.out_energy)
				energy = obs.record_energy(sample, aenergy, options.u, options.j4);
			if (options.out_order)
				ordervals = obs.record_order_parameters(sample, aorder);

			auto end2 = std::chrono::high_resolution_clock::now();

			amonomono.write(ofmonomono);
			aclustercount.write(ofclustercount);
			amonodi.write(ofmonodi);
			atritri.write(oftritri);
			aorder.write(oforder);
			aenergy.write(ofenergy);

			if (total_steps > previous_position_output + options.position_interval)
			{
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
						ofconf << decltype(sample)::bond_t::from_index(j, sample.w) << " ";
				ofconf << std::endl;
				previous_position_output = total_steps;
			}

			if (options.out_histogram)
			{
				write_binary<float>(ofhistogram, (float)energy);
				write_binary<float>(ofhistogram, (float)ordervals.structure_factor_K);
				write_binary<int32_t>(ofhistogram, ordervals.sublattice_polarization);
				ofhistogram.flush();
			}

			if (options.out_winding_histogram)
			{
				write_binary<float>(ofsector, (float)energy);
				auto sector = obs.calculate_topo_sector(sample);
				write_binary<int16_t>(ofsector, (int16_t)std::get<0>(sector));
				write_binary<int16_t>(ofsector, (int16_t)std::get<1>(sector));
				write_binary<int16_t>(ofsector, (int16_t)std::get<2>(sector));
				write_binary<int16_t>(ofsector, (int16_t)std::get<3>(sector));
				ofsector.flush();
			}

			timings[1] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end2 - end1).count();
		}

		timing.record(timings);
	}

	void final_output()
	{
		if (io_initialized)
		{
			amonomono.write(ofmonomono, true);
			aclustercount.write(ofclustercount, true);
			amonodi.write(ofmonodi, true);
			atritri.write(oftritri, true);
			aorder.write(oforder, true);
			aenergy.write(ofenergy, true);
		}
	}
};

double lerp(double x, double a, double b) { return a + x * (b - a); }

struct PTEnsemble
{
	const Options options;

	std::minstd_rand rng;
	std::vector<PTWorker> chains;
	int swap_count = 0;

	Accumulator<double> timing;

	Accumulator<double> acceptance_ratios;
	MovingAverage<double> acceptance_ratios_ma;

	PTEnsemble(const Options& options) : options(options), timing(2, 1)
	{
		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		for (uint i = 0; i < options.temperatures.size(); i++)
			chains.push_back(PTWorker(options, options.temperatures[i], !options.adaptive_pt));

		acceptance_ratios = Accumulator<double>((uint)(chains.size() - 1), 1);
		acceptance_ratios_ma = MovingAverage<double>((uint)(chains.size() - 1), 200);
	}

	int steps_done = 0;
	int last_output = 0;

	static void optimize_schedule(const Accumulator<double>& acceptance_ratios, std::vector<PTWorker>& chains)
	{
		// calculate communication barriers
		std::vector<double> betas;
		std::vector<double> lambdas;

		for (uint i = 0; i < chains.size(); i++)
		{
			betas.push_back(1 / chains[i].temperature);
			if (lambdas.empty())
				lambdas.push_back(0);
			else
				lambdas.push_back(lambdas.back() + (1 - acceptance_ratios.mean[i - 1]));
		}

		int prev_range_begin = 0;
		for (uint i = 1; i < chains.size() - 1; i++)
		{
			double target = lerp(i / ((double)chains.size() - 1), lambdas.front(), lambdas.back());

			auto range_begin =
				std::lower_bound(lambdas.begin() + prev_range_begin, lambdas.end(), target) - lambdas.begin() - 1;

			double new_beta = lerp((target - lambdas[range_begin]) / (lambdas[range_begin + 1] - lambdas[range_begin]),
								   betas[range_begin], betas[range_begin + 1]);

			chains[i].temperature = 1 / new_beta;

			prev_range_begin = (uint)range_begin;
		}
	}

	void pt_swap()
	{
		if (chains.size() < 2)
			return;

		std::uniform_real_distribution<> uniform(0., 1.);
		std::vector<double> acceptances(chains.size() - 1);

		for (uint i = swap_count % 2; i < chains.size() - 1; i += 2)
		{
			chains[i].sample.calculate_energy();
			chains[i + 1].sample.calculate_energy();

			double u_factor = (chains[i].sample.cfg.clusters_total - chains[i + 1].sample.cfg.clusters_total) * options.u;
			double j4_factor = (chains[i].sample.cfg.j4_total - chains[i + 1].sample.cfg.j4_total) * options.j4;

			double action;
			if (u_factor + j4_factor == 0)
				action = 0;
			else
				action = (1 / chains[i + 1].temperature - 1 / chains[i].temperature) * (u_factor + j4_factor);

			double accept = std::exp(-action);
			acceptances[i] += std::min(1., accept);

			if (uniform(rng) < accept)
				std::swap(chains[i].sample.cfg, chains[i + 1].sample.cfg);
		}
		swap_count++;
		acceptance_ratios.record(acceptances);
		acceptance_ratios_ma.record(acceptances);

		if (options.adaptive_pt && steps_done < options.adaptation_duration &&
			acceptance_ratios.total >= options.adaptation_interval)
		{
			optimize_schedule(acceptance_ratios, chains);
			acceptance_ratios.reset();
		}

		if (steps_done >= options.adaptation_duration)
			for (auto& chain : chains)
				chain.enable_io();
	}

	void step_all()
	{
		if (chains.size() == 0)
		{
			std::cout << "no chains" << std::endl;
			throw "";
		}

		std::vector<double> timings(2);

		if (chains.size() > 1)
		{
			std::vector<std::thread> threads;

			auto begin = std::chrono::high_resolution_clock::now();
			for (uint i = 0; i < chains.size(); i++)
				threads.push_back(std::thread([this, i] {
					for (int j = 0; j < options.swap_interval; j++)
						chains[i].step();
				}));
			auto end1 = std::chrono::high_resolution_clock::now();

			for (auto& thread : threads)
				thread.join();
			auto end2 = std::chrono::high_resolution_clock::now();

			timings[0] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
			timings[1] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end2 - end1).count();
		}
		else
		{
			auto begin = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < options.swap_interval; i++)
				chains[0].step();
			auto end = std::chrono::high_resolution_clock::now();

			timings[1] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		}

		steps_done += options.decorr_interval * options.swap_interval;

		if (chains.size() > 1)
			pt_swap();

		timing.record(timings);
		if (steps_done >= last_output + 30000)
		{
			last_output = steps_done;

			std::cout << std::fixed << "Step " << steps_done << " of " << options.total_steps << "; Spawn "
					  << timing.mean[0] << "; Join " << timing.mean[1] << std::endl;
			timing.reset();

			for (uint i = 0; i < chains.size(); i++)
			{
				auto& chain = chains[i];
				std::cout << "Thread " << chain.temperature << "; Ratio (up) "
						  << (i == chains.size() - 1 ? 0 : acceptance_ratios_ma.mean[i]) << "; Step "
						  << chain.timing.mean[0] << "; Measure " << chain.timing.mean[1] << std::endl;
				chain.timing.reset();
			}
		}
	}

	void final_output()
	{
		for (auto& chain : chains)
			chain.final_output();
	}
};

struct MuCaWorker
{
	const Options options;
	using Geometry = TrimerTriangularGeometry;

	Sample<Geometry> sample;
	std::minstd_rand rng;

	std::unordered_map<int, double> log_dos;
	std::unordered_map<int, int> hist;

	double factor;

	int min_energy = 0;
	int max_energy = std::numeric_limits<int>::max();

	std::function<double(int)> default_dos;

	MuCaWorker() : options(Options()) {}

	MuCaWorker(const Options& opt, std::function<double(int)> default_dos) : options(opt), default_dos(default_dos)
	{
		sample = Sample<Geometry>(opt.domain_length, opt.domain_length);

		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		factor = options.multicanonical_init_factor;

		SampleInitializer<Geometry> init;
		init.initialize(sample, opt, rng);
	}

	void set_range(int min, int max)
	{
		min_energy = min;
		max_energy = max;
	}

	void step()
	{
		std::uniform_real_distribution<> uniform(0., 1.);
		for (int i = 0; i < options.decorr_interval; i++)
		{
			sample.calculate_energy();
			Sample<Geometry>::Configuration copy = sample.cfg;
			sample.pocket_move(rng, infinity, 0);
			sample.calculate_energy();

			if (sample.cfg.j4_total > max_energy)
			{
				if (sample.cfg.j4_total < copy.j4_total)
					std::swap(copy, sample.cfg);
				continue;
			}
			if (sample.cfg.j4_total < min_energy)
			{
				if (sample.cfg.j4_total > copy.j4_total)
					std::swap(copy, sample.cfg);
				continue;
			}

			if (uniform(rng) <= 1 - std::exp(get_log_dos(copy.j4_total) - get_log_dos(sample.cfg.j4_total)))
				std::swap(copy, sample.cfg);
		}

		log_dos[sample.cfg.j4_total] += factor;
		hist[sample.cfg.j4_total] += 1;
	}

	double get_log_dos(int j4)
	{
		if (!log_dos.contains(j4))
			return log_dos[j4] = default_dos(j4);
		return log_dos[j4];
	}
};

struct MuCaEnsemble
{
	const Options options;

	std::minstd_rand rng;
	std::vector<MuCaWorker> chains;
	int swap_count = 0;

	double factor = 1;
	Accumulator<double> timing;

	std::ofstream output;
	MovingAverage<double> acceptance_ratios_ma;

	MuCaEnsemble(const Options& options) : options(options), timing(2, 1)
	{
		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		std::filesystem::path basepath;
		int runid = 0;
		do
		{
			std::stringstream ss;
			ss << options.domain_length << "x" << options.domain_length << "_r-" << options.mono_vacancies << "_"
			   << options.multicanonical_round_length << "." << options.decorr_interval << "_" << runid++;
			basepath = std::filesystem::path("new-data") / options.directory / ss.str();
		} while (std::filesystem::exists(basepath));
		std::filesystem::create_directories(basepath);
		std::cout << "saving to " << basepath << std::endl;

		output = std::ofstream(basepath / "log-dos.dat");
		factor = options.multicanonical_init_factor;

		std::function<double(int)> dos_guess;
		dos_guess = [](int) { return 0; };
		if (options.domain_length == 48)
		{
			dos_guess = [](int energy) {
				if (energy < 1140)
					return 194 * std::pow(std::sin(std::pow((double)energy / 1140, 5) * M_PI), 0.5);
				else
					return 0.;
			};
		}

		int max_j4 = options.domain_length * options.domain_length / 2;
		for (int i = 0; i < options.multicanonical_threads; i++)
		{
			int partitions = options.multicanonical_threads + 3;
			chains.push_back(MuCaWorker(options, dos_guess));
			chains.back().set_range((int)lerp((double)i / (double)partitions, 0, max_j4),
									(int)lerp((double)(i + 4) / (double)partitions, 0, max_j4));
		}

		acceptance_ratios_ma = MovingAverage<double>((uint)(chains.size() - 1), 200);
	}

	int steps_done = 0;
	int last_output = 0;
	int last_round = 0;

	void pt_swap()
	{
		if (chains.size() < 2)
			return;

		std::vector<double> acceptances(chains.size() - 1);
		std::uniform_real_distribution<> uniform(0., 1.);
		for (uint i = swap_count % 2; i < chains.size() - 1; i += 2)
		{
			chains[i].sample.calculate_energy();
			chains[i + 1].sample.calculate_energy();

			if (chains[i].sample.cfg.j4_total < chains[i + 1].min_energy)
				continue;
			if (chains[i + 1].sample.cfg.j4_total > chains[i].max_energy)
				continue;

			double accept = std::exp(chains[i].get_log_dos(chains[i].sample.cfg.j4_total) +
									 chains[i + 1].get_log_dos(chains[i + 1].sample.cfg.j4_total) -
									 chains[i].get_log_dos(chains[i + 1].sample.cfg.j4_total) -
									 chains[i + 1].get_log_dos(chains[i].sample.cfg.j4_total));

			if (uniform(rng) < accept)
				std::swap(chains[i].sample.cfg, chains[i + 1].sample.cfg);
		}
		swap_count++;
	}

	void write_out()
	{
		std::cout << std::fixed << "Step " << steps_done << " of " << options.multicanonical_round_length << "; "
				  << chains.size() << " chains; factor=" << factor << "; Spawn " << timing.mean[0] << "; Join "
				  << timing.mean[1] << std::endl;

		timing.reset();

		output << steps_done << " " << factor << " " << chains.size() << std::endl;

		for (const auto& chain : chains)
		{
			int mine = 999999999;
			int maxe = 0;
			int count = 0;
			int minhist = 999999999;
			int maxhist = 0;
			double mindos = 99999999;
			double maxdos = -9999999;
			for (auto& [key, value] : chain.hist)
			{
				if (key >= chain.min_energy && key <= chain.max_energy)
				{
					count++;
					if (value < minhist)
					{
						mine = key;
						mindos = chain.log_dos.at(key);
						minhist = value;
					}
					if (value > maxhist)
					{
						maxe = key;
						maxdos = chain.log_dos.at(key);
						maxhist = value;
					}
				}
			}

			std::cout << chain.min_energy << "-" << chain.max_energy << " " << count << " " << mine << ":" << minhist
					  << ":" << mindos << " -> " << maxe << ":" << maxhist << ":" << maxdos << std::endl;

			output << chain.min_energy << " " << chain.max_energy << std::endl;

			std::vector<std::pair<int, int>> counts(chain.hist.begin(), chain.hist.end());
			std::sort(counts.begin(), counts.end(),
					  [](const std::pair<int, int>& a, const std::pair<int, int>& b) { return a.first < b.first; });

			for (auto& [key, value] : counts)
				output << key << "," << value << " ";
			output << std::endl;

			std::vector<std::pair<int, double>> counts2(chain.log_dos.begin(), chain.log_dos.end());
			std::sort(
				counts2.begin(), counts2.end(),
				[](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.first < b.first; });
			for (auto& [key, value] : counts2)
				output << key << "," << value << " ";
			output << std::endl;
		}
	}

	void step_all()
	{
		if (chains.size() == 0)
		{
			std::cout << "no chains" << std::endl;
			throw "";
		}

		std::vector<double> timings(2);

		if (chains.size() > 1)
		{
			std::vector<std::thread> threads;

			auto begin = std::chrono::high_resolution_clock::now();
			for (uint i = 0; i < chains.size(); i++)
				threads.push_back(std::thread([this, i] {
					for (int j = 0; j < options.multicanonical_swap_interval; j++)
						chains[i].step();
				}));
			auto end1 = std::chrono::high_resolution_clock::now();

			for (auto& thread : threads)
				thread.join();
			auto end2 = std::chrono::high_resolution_clock::now();

			timings[0] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
			timings[1] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end2 - end1).count();
		}
		else
		{
			auto begin = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < options.multicanonical_swap_interval; i++)
				chains[0].step();
			auto end = std::chrono::high_resolution_clock::now();

			timings[1] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		}

		steps_done += options.decorr_interval * options.multicanonical_swap_interval;
		if (chains.size() > 1)
			pt_swap();

		if (steps_done >= last_round + options.multicanonical_round_length)
		{
			for (auto& chain : chains)
			{
				chain.factor *= options.multicanonical_factor_decay;
				for (auto& [key, value] : chain.hist)
					value = 0;
			}
			factor *= options.multicanonical_factor_decay;
			last_round = steps_done;
		}

		timing.record(timings);
		if (steps_done >= last_output + 100000)
		{
			last_output = steps_done;
			write_out();
		}
	}
};

void sim(int argc, char** argv)
{
	if (argc < 10)
	{
		std::cout << "no arguments" << std::endl;
		exit(-1);
	}

	Options options;
	options.domain_length = std::stoi(std::string(argv[1]));
	options.mono_vacancies = std::stoi(std::string(argv[2])) * 3;
	options.u = 1;
	options.j4 = std::stod(std::string(argv[3]));

	std::string temperatures = std::string(argv[4]);
	std::istringstream ss(temperatures);

	if (temperatures.find(',') != std::string::npos)
	{
		std::string token;
		while (std::getline(ss, token, ','))
			options.temperatures.push_back(std::stod(token));
	}
	else if (temperatures.find(':') != std::string::npos)
	{
		std::string token;
		std::getline(ss, token, ':');
		double begin = std::stod(token);
		std::getline(ss, token, ':');
		int count = std::stoi(token);
		std::getline(ss, token, ':');
		double end = std::stod(token);

		for (int i = 0; i < count; i++)
			options.temperatures.push_back(lerp(i / (double)(count - 1), begin, end));
	}
	else
	{
		options.temperatures.push_back(std::stod(temperatures));
	}

	options.total_steps = std::stoi(std::string(argv[5]));
	options.decorr_interval = std::stoi(std::string(argv[6]));

	std::string optstring = std::string(argv[7]);
	options.accumulator_interval = std::stoi(std::string(argv[8]));

	options.directory = std::string(argv[9]);

	options.out_monomono = optstring.find('m') != std::string::npos;
	options.out_clustercount = optstring.find('c') != std::string::npos;
	options.out_monodi = optstring.find('d') != std::string::npos;
	options.out_tritri = optstring.find('t') != std::string::npos;
	options.out_energy = optstring.find('e') != std::string::npos;
	options.out_order = optstring.find('o') != std::string::npos;
	options.out_histogram = optstring.find('h') != std::string::npos;
	options.out_winding_histogram = optstring.find('w') != std::string::npos;

	if (optstring.find('I') != std::string::npos)
		options.u = infinity;

	if (options.out_histogram)
	{
		options.out_order = true;
		options.out_energy = true;
	}

	options.idealbrickwall = optstring.find('B') != std::string::npos;
	options.idealrt3 = optstring.find('3') != std::string::npos;
	options.metropolis = optstring.find('M') != std::string::npos;
	options.adaptive_pt = optstring.find('A') != std::string::npos;
	options.init_random = optstring.find('R') != std::string::npos;

	options.multicanonical = optstring.find('C') != std::string::npos;
	options.multicanonical_round_length = options.total_steps;
	options.multicanonical_threads = options.accumulator_interval;

#ifdef ERGOTEST
	options.pocket_fraction = options.accumulator_interval / 100.;
#endif

	if (optstring.find('D') != std::string::npos)
	{
		std::minstd_rand rng;
		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		using Geometry = DimerSquareGeometry;
		std::vector<Geometry::bond_t> positions;
		for (int i = 0; i < options.domain_length; i += 2)
			for (int j = 0; j < options.domain_length; j += 1)
				positions.push_back(Geometry::bond_t(i, j, 0));

		Sample<Geometry> sample(options.domain_length, options.domain_length, positions);
		Observer<Geometry> obs;

		auto path = std::filesystem::path("new-data") / options.directory;
		std::filesystem::create_directories(path);
		std::ofstream ofhist(path / "winding-histogram.dat", std::ios::binary);
		std::ofstream ofconf(path / "positions.dat");

		std::uniform_real_distribution<> uniform(0., 1.);

		int nstep = 0;
		int prev_conf = 0;
		while (nstep < options.total_steps)
		{
			for (int s = 0; s < options.decorr_interval; s++)
			{
				if (uniform(rng) < options.pocket_fraction)
					sample.pocket_move(rng, infinity, 0);
				else
					sample.worm_move(rng, infinity);
			}

			nstep += options.decorr_interval;

			auto windings = obs.calculate_topo_sector(sample);
			write_binary<int16_t>(ofhist, (int16_t) windings.first);
			write_binary<int16_t>(ofhist, (int16_t) windings.second);
			ofhist.flush();

			if (nstep >= prev_conf + options.position_interval)
			{
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
						ofconf << decltype(sample)::bond_t::from_index(j, sample.w) << " ";
				ofconf << std::endl;
				prev_conf = nstep;
				std::cout << nstep << std::endl;
			}
		}
	}
	if (optstring.find('H') != std::string::npos)
	{
		std::minstd_rand rng;
		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		using Geometry = DimerHexagonalSkewGeometry;
		std::vector<Geometry::bond_t> positions;
		for (int i = 0; i < options.domain_length; i += 1)
			for (int j = 0; j < options.accumulator_interval; j += 1)
				positions.push_back(Geometry::bond_t(i, j, 0));

		Sample<Geometry> sample(options.domain_length, options.accumulator_interval, positions);
		Observer<Geometry> obs;

		auto path = std::filesystem::path("new-data") / options.directory;
		std::filesystem::create_directories(path);
		std::ofstream ofhist(path / "winding-histogram.dat", std::ios::binary);
		std::ofstream ofconf(path / "positions.dat");

		int nstep = 0;
		int prev_conf = 0;

		std::uniform_real_distribution<> uniform(0., 1.);
		while (nstep < options.total_steps)
		{
			for (int s = 0; s < options.decorr_interval; s++)
			{
				if (uniform(rng) < options.pocket_fraction)
					sample.pocket_move(rng, infinity, 0);
				else
					sample.worm_move(rng, infinity);
			}

			nstep += options.decorr_interval;

			auto windings = obs.calculate_topo_sector(sample);
			write_binary<int16_t>(ofhist, (int16_t) windings.first);
			write_binary<int16_t>(ofhist, (int16_t) windings.second);
			ofhist.flush();

			if (nstep >= prev_conf + options.position_interval)
			{
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
						ofconf << decltype(sample)::bond_t::from_index(j, sample.w) << " ";
				ofconf << std::endl;
				prev_conf = nstep;
				std::cout << nstep << std::endl;
			}
		}
	}
	else
	{
		if (options.multicanonical)
		{
			MuCaEnsemble ens(options);
			while (true)
				ens.step_all();
		}
		else
		{
			PTEnsemble ensemble(options);
			while (ensemble.steps_done < options.total_steps)
				ensemble.step_all();
			ensemble.final_output();
		}
	}
}

void test_optimizer()
{
	std::vector<PTWorker> chains(3);
	chains[0].temperature = 1;
	chains[1].temperature = 2;
	chains[2].temperature = 3;

	Accumulator<double> acceptance_ratios(2, 1);
	acceptance_ratios.mean = std::vector<double>{0.5, 0};

	PTEnsemble::optimize_schedule(acceptance_ratios, chains);

	// betas = [1, 0.5, 0.33]
	// communication barriers = [0, 0.5, 1.5]
	// optimized betas = [1, 0.4583, 0.33]
	// optimized temperatures = [1, 2.1815, 3]
	TEST_ASSERT2(std::abs(chains[1].temperature - 2.1815) < 1e-2, chains[1].temperature)
}

void test_pocket1()
{
	SampleInitializer<TrimerTriangularGeometry> init;
	Sample<TrimerTriangularGeometry> sample(6, 6);
	std::minstd_rand rng;
	init.reconfigure_brick_wall(sample, rng);

	sample.calculate_energy();
	TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.pocket_move(rng, infinity, 0);
		sample.calculate_energy();
		TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

		int ntri = 0;
		for (auto j : sample.cfg.bond_occ)
			if (j)
				ntri++;
		TEST_ASSERT2(ntri == 6 * 6 / 3, ntri);
	}

	std::cout << "pocket test finished with avg length " << total / 300. << std::endl;

	total = 0;
	for (int i = 0; i < 300; i++)
		total += sample.pocket_move(rng, 1, 0.01);

	std::cout << "pocket test finished with avg length " << total / 300. << std::endl;
}

void test_pocket2()
{
	SampleInitializer<TrimerTriangularGeometry> init;
	Sample<TrimerTriangularGeometry> sample(12, 12);
	std::minstd_rand rng;
	init.reconfigure_root3(sample, rng);

	sample.calculate_energy();
	TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

	int nj4 = sample.cfg.j4_total;

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.pocket_move(rng, infinity, infinity);
		sample.calculate_energy();
		std::cout << sample.cfg.j4_total << std::endl;
		TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);
		TEST_ASSERT2(sample.cfg.j4_total == nj4, sample.cfg.clusters_total);
	}
}

void test_pocket3()
{
	SampleInitializer<TrimerTriangularGeometry> init;
	Sample<TrimerTriangularGeometry> sample(12, 12);
	std::minstd_rand rng;
	init.reconfigure_brick_wall(sample, rng);

	sample.calculate_energy();
	TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

	int nj4 = sample.cfg.j4_total;

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.pocket_move(rng, infinity, -infinity);
		sample.calculate_energy();
		TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);
		TEST_ASSERT2(sample.cfg.j4_total == nj4, sample.cfg.clusters_total);
	}
}

void test_worm1()
{
	SampleInitializer<TrimerTriangularGeometry> init;
	Sample<TrimerTriangularGeometry> sample(6, 6);
	std::minstd_rand rng;
	init.reconfigure_brick_wall(sample, rng);

	sample.calculate_energy();
	TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.worm_move(rng, infinity);
		sample.calculate_energy();
		TEST_ASSERT2(sample.cfg.clusters_total == 0, sample.cfg.clusters_total);

		int ntri = 0;
		for (auto j : sample.cfg.bond_occ)
			if (j)
				ntri++;
		TEST_ASSERT2(ntri == 6 * 6 / 3, ntri);
	}

	std::cout << "worm test finished with avg length " << total / 300. << std::endl;

	total = 0;
	for (int i = 0; i < 300; i++)
		total += sample.worm_move(rng, 1);

	std::cout << "worm test finished with avg length " << total / 300. << std::endl;
}

void test_brickwall()
{
	SampleInitializer<TrimerTriangularGeometry> init;
	Sample<TrimerTriangularGeometry> sample(48, 48);

	std::minstd_rand rng;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 20; j++)
			init.reconfigure_brick_wall(sample, rng);

		sample.calculate_energy();
		TEST_ASSERT2(sample.cfg.j4_total == 48 * 48 / 2, sample.cfg.j4_total)
	}
}

void test_symmetry()
{
	using Geom = TrimerTriangularGeometry;

	int w = 12;
	Geom geom(w, w);

	for (int d = 0; d < 17; d++)
	{
		std::vector<bool> occupancies(w * w * 2);
		for (int i = 0; i < w; i++)
			for (int j = 0; j < w; j++)
				for (int s = 0; s < 2; s++)
					occupancies[geom.apply_symmetry(Geom::bond_t(i, j, (int8_t) s), Geom::vertex_t(1, 2), d).index(w)] = true;

		TEST_ASSERT(std::find(occupancies.begin(), occupancies.end(), false) == occupancies.end());
	}
}

void test_symmetry_hexa()
{
	using Geom = DimerHexagonalSkewGeometry;

	int w = 12;
	Geom geom(w, w);
	
	for (int d = 0; d < 6; d++)
	{
		std::vector<bool> occupancies(w * w * 3);
		for (int i = 0; i < w; i++)
			for (int j = 0; j < w; j++)
				for (int s = 0; s < 3; s++)
					occupancies[geom.apply_symmetry(Geom::bond_t(i, j, (int8_t) s), LatticePos(1, 2), d).index(w)] = true;

		TEST_ASSERT(std::find(occupancies.begin(), occupancies.end(), false) == occupancies.end());
	}
}

#ifndef TEST
int main(int argc, char** argv) { sim(argc, argv); }
#else
int main()
{
	test_pocket1();
	test_pocket2();
	test_pocket3();

	test_worm1();
	test_brickwall();
	test_symmetry();
	test_symmetry_hexa();
	test_optimizer();
}
#endif
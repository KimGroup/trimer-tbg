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
		std::cout << "test failed at " << __LINE__ << #x << std::endl;                                                 \
		assert_fail();                                                                                                 \
	}                                                                                                                  \
	else                                                                                                               \
	{                                                                                                                  \
		std::cout << "test passed at " << __LINE__ << #x << std::endl;                                                 \
	}
#define TEST_ASSERT2(x, y)                                                                                             \
	if (!(x))                                                                                                          \
	{                                                                                                                  \
		std::cout << "test failed at " << __LINE__ << #x << "; " << (y) << std::endl;                                  \
		assert_fail();                                                                                                 \
	}                                                                                                                  \
	else                                                                                                               \
	{                                                                                                                  \
		std::cout << "test passed at " << __LINE__ << #x << "; " << (y) << std::endl;                                  \
	}

#else
#define ASSERT(x)
#define TEST_ASSERT(x)
#define TEST_ASSERT2(x, y)
#endif

int32_t pmod(int32_t a, int32_t b)
{
	int32_t m = a % b;
	if (m < 0) m += b;
	return m;
}

const double infinity = std::numeric_limits<double>::infinity();
using complex = std::complex<double>;

struct VertexPos
{
	int32_t x, y;

	VertexPos() : x(0), y(0) {}
	VertexPos(int32_t x, int32_t y) : x(x), y(y) {}

	VertexPos rotate(int dir)
	{
		switch (pmod(dir, 6))
		{
		default:
		case 0:
			return VertexPos(x, y);
		case 1:
			return VertexPos(-y, x + y);
		case 2:
			return VertexPos(-x - y, x);
		case 3:
			return VertexPos(-x, -y);
		case 4:
			return VertexPos(y, -x - y);
		case 5:
			return VertexPos(x + y, -x);
		}
	}

	VertexPos canonical(int32_t w, int32_t h) const { return VertexPos(pmod(x, w), pmod(y, h)); }
	VertexPos center_at(VertexPos pos, int32_t w, int32_t h) const
	{
		return VertexPos(pmod(x - pos.x + w / 2, w) - w / 2, pmod(y - pos.y + h / 2, h) - h / 2);
	}

	uint32_t index(int w) const { return x + y * w; }
	static VertexPos from_index(uint32_t index, int w) { return VertexPos(index % w, index / w); }

	VertexPos operator-() const { return VertexPos(-x, -y); }
	VertexPos operator-(const VertexPos& other) const { return VertexPos(x - other.x, y - other.y); }
	VertexPos operator+(const VertexPos& other) const { return VertexPos(x + other.x, y + other.y); }
	bool operator==(const VertexPos& other) const { return x == other.x && y == other.y; }
};

struct BondPos
{
	int32_t x, y;
	int8_t s;

	BondPos() : x(0), y(0), s(0) {}
	BondPos(int x, int y, int s) : x(x), y(y), s((int8_t)s) {}

	VertexPos mono_pos() const { return VertexPos(x, y); }

	// rotate around the plaquette at (0, 0)
	BondPos rotate(int dir)
	{
		VertexPos p = VertexPos(x, y);
		switch (pmod(dir, 6))
		{
		case 0:
			return *this;
		case 1:
			p = p.rotate(1);
			return !s ? BondPos(p.x-1, p.y, 1) : BondPos(p.x-1, p.y+1, 0);
		case 2:
			p = p.rotate(2);
			return !s ? BondPos(p.x-1, p.y, 0) : BondPos(p.x-2, p.y, 1);
		case 3:
			p = p.rotate(3);
			return !s ? BondPos(p.x-1, p.y-1, 1) : BondPos(p.x-1, p.y-1, 0);
		case 4:
			p = p.rotate(4);
			return !s ? BondPos(p.x, p.y-1, 0) : BondPos(p.x, p.y-2, 1);
		case 5:
			p = p.rotate(5);
			return !s ? BondPos(p.x, p.y-1, 1) : BondPos(p.x+1, p.y-1, 0);
		default:
			return *this;
		}
	}

	BondPos reflect(int dir) const
	{
		// Reflect about the vertex (0,0)
		switch (dir)
		{
		case 0:
			return !s ? BondPos(x+y, -y-1, 1) : BondPos(x+y+1, -y-1, 0);
		case 1:
			return !s ? BondPos(y, x, 0) : BondPos(y, x, 1);
		case 2:
			return !s ? BondPos(-x-1, y+x, 1) : BondPos(-x- 1, y+x+1, 0);
		case 3:
			return !s ? BondPos(-x-y-1, y, 0) : BondPos(-x-y-2, y, 1);
		case 4:
			return !s ? BondPos(-y-1, -x-1, 1) : BondPos(-y - 1, -x-1, 0);
		case 5:
			return !s ? BondPos(x, -y-x-1, 0) : BondPos(x, -y-x-2, 1);
		default:
			return *this;
		}
	}

	BondPos apply_symmetry(VertexPos center, int index, int w, int h) const
	{
		auto p = center_at(center, w, h);

		if (index < 12)
			p = p.reflect(index % 6);
		else if (index < 17)
			p = p.rotate(index % 6 + 1);
		else if (index == 17)
			// pi rotation 1
			p = BondPos(-p.x, -p.y, 1-p.s);
		else if (index == 18)
			// pi rotation 2
			p = BondPos(-p.x-1, -p.y, 1-p.s);
		else if (index == 19)
			// pi rotation 3
			p = BondPos(-p.x, -p.y-1, 1-p.s);
		else if (index == 20)
			// 2pi/3 around A
			p = BondPos(-p.x-p.y-p.s, p.x, p.s);
		else if (index == 21)
			// -2pi/3 around A
			p = BondPos(p.y, -p.x-p.y-p.s, p.s);
		else if (index == 22)
			// 2pi/3 around B
			p = BondPos(-p.x-p.y+1-p.s, p.x, p.s);
		else if (index == 23)
			// -2pi/3 around B
			p = BondPos(p.y, -p.x-p.y+1-p.s, p.s);

		return BondPos(center.x + p.x, center.y + p.y, p.s).canonical(w, h);
	}

	BondPos canonical(int32_t w, int32_t h) const { return BondPos(pmod(x, w), pmod(y, h), s); }
	BondPos center_at(VertexPos pos, int32_t w, int32_t h) const
	{
		return BondPos(pmod(x - pos.x + w / 2, w) - w / 2, pmod(y - pos.y + h / 2, h) - h / 2, s);
	}

	uint32_t index(int w) const { return (x + y * w) * 2 + s; }
	static BondPos from_index(uint32_t index, int w)
	{
		uint32_t half = index / 2;
		return BondPos(half % w, half / w, index % 2);
	}

	bool operator==(const BondPos& other) const { return x == other.x && y == other.y && s == other.s; }
	BondPos operator+(const VertexPos& other) const { return BondPos(x + other.x, y + other.y, s); }
};

struct DimerGeometryProvider
{
	std::array<VertexPos, 2> get_vertices(BondPos pos, int w, int h) const
	{
		std::array<VertexPos, 2> r;
		r[0] = {pos.x, pos.y};

		if (pos.s == 0)
			r[1] = {pos.x + 1, pos.y};
		else
			r[1] = {pos.x, pos.y + 1};

		
		r[0] = r[0].canonical(w, h);
		r[1] = r[1].canonical(w, h);

		return r;
	}

	std::array<BondPos, 4> vertex_dimers = {
		BondPos{0, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, -1, 1}
	};

	std::array<BondPos, 4> get_bonds(VertexPos pos, int w, int h) const
	{
		std::array<BondPos, 4> r;

		for (int i = 0; i < 4; i++)
			r[i] = (vertex_dimers[i] + pos).canonical(w, h);

		return r;
	}
};

struct TrimerGeometryProvider
{
	std::array<VertexPos, 3> get_vertices(BondPos pos, int w, int h) const
	{
		std::array<VertexPos, 3> r;

		if (pos.s == 0)
		{
			r[0] = {pos.x, pos.y};
			r[1] = {pos.x + 1, pos.y};
			r[2] = {pos.x, pos.y + 1};
		}
		else
		{
			r[0] = {pos.x + 1, pos.y};
			r[1] = {pos.x + 1, pos.y + 1};
			r[2] = {pos.x, pos.y + 1};
		}

		r[0] = r[0].canonical(w, h);
		r[1] = r[1].canonical(w, h);
		r[2] = r[2].canonical(w, h);

		return r;
	}

	std::array<BondPos, 6> vertex_trimers = {
		BondPos{0, 0, 0}, {-1, 0, 1}, {-1, 0, 0},
		{-1, -1, 1}, {0, -1, 0}, {0, -1, 1}
	};

	std::array<BondPos, 6> get_bonds(VertexPos pos, int w, int h) const
	{
		std::array<BondPos, 6> r;

		for (int i = 0; i < 6; i++)
			r[i] = (vertex_trimers[i] + pos).canonical(w, h);

		return r;
	}

	std::array<VertexPos, 6> mono_flips = {
		VertexPos{1, 1}, {-1, 2}, {-2, 1}, {-1, -1}, {1, -2}, {2, -1}
	};
	std::array<BondPos, 6> tri_flips = {
		BondPos{0, 0, 1}, {-1, 1, 0}, {-2, 0, 1},
		{-1, -1, 0}, {0, -2, 1}, {1, -1, 0}
	};

	std::array<std::array<VertexPos, 6>, 2> j4_neighbor_list = {
		std::array<VertexPos, 6>{VertexPos{0, 1}, {1, 0}, {0, -2}, {1, -2}, {-2, 0}, {-2, 1}},
		{VertexPos{0, 2}, {-1, 2}, {2, 0}, {2, -1}, {0, -1}, {-1, 0}}};

	std::array<VertexPos, 3> j6_neighbor_list = std::array<VertexPos, 3>{
		VertexPos{0, 2}, {2, 0}, {-2, 2}
	};
};

namespace std
{
template <> struct hash<VertexPos>
{
	std::size_t operator()(const VertexPos& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
template <> struct hash<BondPos>
{
	std::size_t operator()(const BondPos& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(x.s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
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

std::ostream& operator<<(std::ostream& o, const VertexPos& v) { return o << "(" << v.x << "," << v.y << ")"; }
std::ostream& operator<<(std::ostream& o, const BondPos& v)
{
	return o << "(" << v.x << "," << v.y << "," << (int)v.s << ")";
}

template <typename T> struct Accumulator
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
	int adaptation_duration = 800000;

	double j4 = 0;
	double u = infinity;
	std::vector<double> temperatures;
	bool adaptive_pt = false;

	std::string directory;
};

struct InteractionCount
{
	int u = 0;
	int j4 = 0;
	int hard_core = 0;
};

struct Sample
{
  private:
	std::vector<BondPos> _pocket, _Abar;
	std::unordered_map<BondPos, InteractionCount> _candidates;
	std::unordered_map<int, std::vector<complex>> _ft_contributions;

  public:
	int32_t w, h;
	TrimerGeometryProvider geom;

	struct Configuration
	{
		std::vector<bool> bond_occ;
		std::vector<uint8_t> vertex_occ;
		int j4_total, clusters_total;
	};

	Configuration cfg;

	Sample() : w(0), h(0)
	{
		cfg.j4_total = 0;
		cfg.clusters_total = 0;
	}

	Sample(int32_t w, int32_t h) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 2, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h, 0);
		cfg.j4_total = 0;
		cfg.clusters_total = 0;
	}

	Sample(int32_t w, int32_t h, const std::vector<BondPos>& trimers) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 2, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h, 0);

		for (auto& i : trimers)
			cfg.bond_occ[i.index(w)] = true;

		cfg.j4_total = -1;
		cfg.clusters_total = -1;

		regenerate_occupation();
		calculate_energy();
	}

	template <typename Rng> void initialize(const Options& opt, Rng& rng)
	{
		if (opt.init_random)
		{
			std::ostringstream ss;
			ss << "new-data/disordered/" << opt.domain_length << "x" << opt.domain_length
			   << "_r-0_t0.000000_j0.000_20000000.200_0/positions.dat";
			load_from(ss.str(), rng, opt.mono_vacancies / 3);
		}
		else
		{
			if (opt.j4 < 0)
				reconfigure_brick_wall(rng, opt.mono_vacancies / 3);
			else
				reconfigure_root3(rng, opt.mono_vacancies / 3);
		}
	}

	void regenerate_occupation()
	{
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		for (uint i = 0; i < cfg.bond_occ.size(); i++)
			if (cfg.bond_occ[i])
			{
				auto tri = BondPos::from_index(i, w);
				for (auto& vtx : geom.get_vertices(tri, w, h))
					cfg.vertex_occ[vtx.index(w)]++;
			}
	}

	void decrease_j4_bonds(const BondPos& removed)
	{
		for (auto& rel : geom.j4_neighbor_list[removed.s])
			if (cfg.bond_occ[BondPos(removed.x + rel.x, removed.y + rel.y, 1 - removed.s).canonical(w, h).index(w)])
				cfg.j4_total--;
	}

	void increase_j4_bonds(const BondPos& added)
	{
		for (auto& rel : geom.j4_neighbor_list[added.s])
			if (cfg.bond_occ[BondPos(added.x + rel.x, added.y + rel.y, 1 - added.s).canonical(w, h).index(w)])
				cfg.j4_total++;
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
				total_occ -= 3;

		ASSERT(total_occ == 0);
#endif

		for (const auto& occ : cfg.vertex_occ)
			cfg.clusters_total += occ * (occ - 1) / 2;

		for (int x = 0; x < w; x++)
			for (int y = 0; y < h; y++)
				if (cfg.bond_occ[BondPos(x, y, 0).index(w)])
					increase_j4_bonds(BondPos(x, y, 0));
	}

	template <typename Rng>
	void record_dimer_monomer_correlations(Accumulator<double>& a, Rng& rng, int max_dimers) const
	{
		std::vector<double> vals(w * h);

		std::vector<std::pair<VertexPos, int>> dimers;
		std::unordered_set<VertexPos> isolated_monos;
		std::unordered_set<VertexPos> connected_monos;

		const std::array<VertexPos, 3> neighbors = {VertexPos{1, 0}, {0, 1}, {-1, 1}};

		for (uint i = 0; i < cfg.vertex_occ.size(); i++)
		{
			if (cfg.vertex_occ[i] > 0)
				continue;

			bool connected = false;
			auto pos = VertexPos::from_index(i, w);
			for (int r = 0; r < 3; r++)
			{
				auto neighbor = (pos + neighbors[r]).canonical(w, h);
				if (cfg.vertex_occ[neighbor.index(w)] == 0)
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
				vals[j.center_at(dimer.first, w, h).rotate(-dimer.second).canonical(w, h).index(w)]++;
		}

		a.record(vals);
	}

	double trimer_correlation(BondPos d) const
	{
		int total = 0;
		int found = 0;

		for (uint i = 0; i < cfg.bond_occ.size(); i++)
		{
			if (cfg.bond_occ[i])
			{
				BondPos p1 = BondPos::from_index(i, w);
				BondPos n;

				if (p1.s == 1)
					n = BondPos(-p1.x + d.x, -p1.y + d.y, 1 - d.s);
				else
					n = BondPos(p1.x + d.x, p1.y + d.y, d.s);

				if (cfg.bond_occ[n.canonical(w, h).index(w)])
					found++;
				total++;
			}
		}

		return (double)found / total;
	}


	void record_trimer_correlations(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h * 2);
		for (uint i = 0; i < cfg.bond_occ.size(); i++)
		{
			if (cfg.bond_occ[i])
				for (uint j = i + 1; j < cfg.bond_occ.size(); j++)
					if (cfg.bond_occ[j])
					{
						auto p1 = BondPos::from_index(i, w);
						auto p2 = BondPos::from_index(j, w);

						auto d = p1.s == 0 ? BondPos(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : BondPos(p1.x - p2.x, p1.y - p2.y, 1 - p2.s);

						vals[d.canonical(w, h).index(w)]++;
					}
		}

		a.record(vals);
	}

	template <typename Rng>
	void record_partial_trimer_correlations(Accumulator<double>& a, Rng& rng, double fraction) const
	{
		std::vector<double> vals(w * h * 2);
		std::uniform_real_distribution<> uniform(0., 1.);

		int seen = 0;
		for (uint i = 0; i < cfg.bond_occ.size(); i++)
		{
			if (cfg.bond_occ[i] && uniform(rng) <= fraction)
			{
				seen++;
				for (uint j = 0; j < cfg.bond_occ.size(); j++)
					if (cfg.bond_occ[j])
					{
						if (j == i)
							continue;

						auto p1 = BondPos::from_index(i, w);
						auto p2 = BondPos::from_index(j, w);
						if (j > i)
							std::swap(p1, p2);

						auto d = p1.s == 0 ? BondPos(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : BondPos(p1.x - p2.x, p1.y - p2.y, 1 - p2.s);

						vals[d.canonical(w, h).index(w)]++;
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

	struct Observables record_order_parameters(Accumulator<double>& acc)
	{
		std::vector<double> vals(11);

		const double rt3 = std::sqrt(3.);
		const double Kpx0 = 4. / 3, Kpy0 = 0;
		// const double Kpx1 = -2. / 3, Kpy1 = 2 * rt3 / 3;
		// const double Kpx2 = -2. / 3, Kpy2 = -2 * rt3 / 3;
		const double Kpx1 = 4. / 3, Kpy1 = 4. / rt3 / (double)h;
		const double Kpx2 = 4. / 3, Kpy2 = 8. / rt3 / (double)w;

		const double Mpx0 = 0, Mpy0 = 2 * rt3 / 3;
		const double Mpx1 = 1, Mpy1 = -rt3 / 3;
		const double Mpx2 = -1, Mpy2 = -rt3 / 3;

		complex M0, M1, M2, K0, K1, K2;

		int AmB = 0;

		for (int i = 0; i < (int)cfg.bond_occ.size(); i++)
		{
			if (cfg.bond_occ[i])
			{
				auto pos = BondPos::from_index(i, w);
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
	void record_partial_monomer_correlations(Accumulator<double>& a, Rng& rng, int max_count) const
	{
		std::vector<double> vals(w * h);
		std::vector<VertexPos> pos;

		for (uint i = 0; i < cfg.vertex_occ.size(); i++)
			if (cfg.vertex_occ[i] == 0)
				pos.push_back(VertexPos::from_index(i, w));

		std::shuffle(pos.begin(), pos.end(), rng);
		uint sample_count = (uint)std::min(pos.size(), (size_t)max_count);

		for (uint i = 0; i < sample_count; i++)
		{
			for (uint j = 0; j < pos.size(); j++)
			{
				if (j == i)
					continue;

				auto d = pos[i] - pos[j];
				vals[d.canonical(w, h).index(w)] += 0.5;
				vals[(-d).canonical(w, h).index(w)] += 0.5;
			}
		}

		double inv_count = 1. / sample_count;
		for (auto& x : vals)
			x *= inv_count;

		a.record(vals);
	}

	void record_monomer_correlations(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h);
		std::vector<VertexPos> pos;

		for (uint i = 0; i < cfg.vertex_occ.size(); i++)
			if (cfg.vertex_occ[i] == 0)
				pos.push_back(VertexPos::from_index(i, w));

		for (uint i = 0; i < pos.size(); i++)
		{
			for (uint j = i + 1; j < pos.size(); j++)
			{
				auto d = pos[i] - pos[j];
				vals[d.canonical(w, h).index(w)] += 1;
				vals[(-d).canonical(w, h).index(w)] += 1;
			}
		}

		double inv_count = 1. / (double)pos.size();
		for (auto& x : vals)
			x *= inv_count;

		a.record(vals);
	}

	void record_cluster_count(Accumulator<double>& a) const
	{
		std::vector<double> vals(7);

		for (uint i = 0; i < cfg.vertex_occ.size(); i++)
			vals[cfg.vertex_occ[i]]++;

		a.record(vals);
	}

	double record_energy(Accumulator<double>& a, double u, double j4)
	{
		std::vector<double> vals(5);
		calculate_energy();

		vals[0] = cfg.clusters_total;
		vals[1] = cfg.j4_total;

		if (u < infinity)
			vals[2] = cfg.clusters_total * u + cfg.j4_total * j4;
		else
			vals[2] = cfg.j4_total * j4;

		vals[3] = vals[2] * vals[2];
		vals[4] = vals[3] * vals[3];

		a.record(vals);

		return vals[2];
	}

	int calculate_winding_number(BondPos start, int dir) const
	{
		BondPos cpos = start;
		int tot = -(w + h) / 6;

		while (true)
		{
			if (cfg.bond_occ[cpos.index(w)])
				tot++;

			switch (dir % 3)
			{
			case 0:
				if (cpos.s == 0)
					cpos = BondPos(cpos.x, cpos.y, 1);
				else
					cpos = BondPos(cpos.x + 1, cpos.y + 1, 0);
				break;
			case 1:
				if (cpos.s == 0)
					cpos = BondPos(cpos.x - 1, cpos.y, 1);
				else
					cpos = BondPos(cpos.x - 1, cpos.y + 1, 0);
				break;
			case 2:
				if (cpos.s == 0)
					cpos = BondPos(cpos.x, cpos.y - 1, 1);
				else
					cpos = BondPos(cpos.x + 1, cpos.y - 1, 0);
				break;
			}

			cpos = cpos.canonical(w, h);

			if (cpos == start)
				break;
		}
		return tot;
	}

	std::tuple<int, int, int, int> calculate_topo_sector() const
	{
		int wa = calculate_winding_number(BondPos(0, 0, 0), 0);
		int wb = calculate_winding_number(BondPos(2, 0, 0), 1);
		int wc = calculate_winding_number(BondPos(1, 0, 0), 0);
		int wd = calculate_winding_number(BondPos(0, 0, 0), 1);

		int ri = (wa - wb) / 3, rj = (2 * wa + wb) / 3;
		int gi = (wc - wd) / 3, gj = (2 * wc + wd) / 3;
		return std::make_tuple(ri, rj, gi, gj);
	}

	void set_pocket_candidates(const BondPos& el, const BondPos& moved, double u, double j4)
	{
		_candidates.clear();

		// hard-core constraint
		if (cfg.bond_occ[moved.index(w)])
			_candidates[moved].hard_core++;

		// add target overlaps to candidates
		if (u != 0)
			for (const auto& i : geom.get_vertices(moved, w, h))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i, w, h))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates[trimer_pos].u++;

		// add source overlaps to candidates
		if (u != 0 && u < infinity)
			for (const auto& i : geom.get_vertices(el, w, h))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i, w, h))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates[trimer_pos].u--;

		if (j4 != 0)
		{
			for (auto& d : geom.j4_neighbor_list[el.s])
			{
				auto pos = (BondPos(el.x, el.y, 1 - el.s) + d).canonical(w, h);
				if (cfg.bond_occ[pos.index(w)])
					_candidates[pos].j4--;
			}

			for (auto& d : geom.j4_neighbor_list[moved.s])
			{
				auto pos = (BondPos(moved.x, moved.y, 1 - moved.s) + d).canonical(w, h);
				if (cfg.bond_occ[pos.index(w)])
					_candidates[pos].j4++;
			}
		}
	}

	template <typename Rng> int pocket_move(Rng& rng, double u, double j4)
	{
		calculate_energy();

		VertexPos symc;
		int syma;
		bool symmetry_is_involute = false;

		BondPos seed(0, 0, 2);
		while (seed.s == 2)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());

			symc = VertexPos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % 17);

			if (cfg.bond_occ[candidate])
			{
				seed = BondPos::from_index(candidate, w);
				if (symmetry_is_involute && cfg.bond_occ[seed.apply_symmetry(symc, syma, w, h).index(w)])
					seed = BondPos(0, 0, 2);
			}
		}

		std::uniform_real_distribution<> uniform(0., 1.);

		_Abar.clear();
		_pocket.clear();

		_pocket.push_back(seed);

		cfg.bond_occ[seed.index(w)] = false;
		for (auto& i : geom.get_vertices(seed, w, h))
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

			auto moved = el.apply_symmetry(symc, syma, w, h);
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
					for (const auto& vtx : geom.get_vertices(pos, w, h))
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

			for (auto& vtx : geom.get_vertices(i, w, h))
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

	void worm_flip(VertexPos& pos, BondPos& moving, const BondPos& move_from, int direction)
	{
		ASSERT(cfg.bond_occ[move_from.index(w)]);
		cfg.bond_occ[move_from.index(w)] = false;
		decrease_j4_bonds(move_from);

		if (moving.s >= 0)
		{
			ASSERT(!cfg.bond_occ[moving.index(w)]);
			cfg.bond_occ[moving.index(w)] = true;
			increase_j4_bonds(moving);
		}

		moving = (geom.tri_flips[direction] + pos).canonical(w, h);

		if (cfg.bond_occ[moving.index(w)])
			// hit a hard-core constraint; force reversal
			moving = move_from;
		else
			pos = (geom.mono_flips[direction] + pos).canonical(w, h);
	}

	template <typename Rng> void worm_init(Rng& rng, VertexPos& cur_pos, BondPos& moving, const BondPos& move_from)
	{
		auto vertices = geom.get_vertices(move_from, w, h);
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
		worm_flip(cur_pos, moving, move_from, dir * 2 + move_from.s);
	}

	template <typename Rng> int worm_move(Rng& rng, double u)
	{
		calculate_energy();

		BondPos seed(0, 0, -1);
		while (seed.s == -1)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (cfg.bond_occ[candidate])
				seed = BondPos::from_index(candidate, w);
		}

		// init to invalid value so that worm_init does not exclude any direction
		VertexPos pos(-1, -1);

		// holds the temporary position of the moving trimer
		BondPos moving(0, 0, -1);

		worm_init(rng, pos, moving, seed);

		std::uniform_real_distribution<> uniform(0., 1.);
		int length = 0;

		while (true)
		{
			length++;

			// hard-core; force reversal
			if (cfg.bond_occ[moving.index(w)])
			{
				worm_init(rng, pos, moving, moving);
				continue;
			}

			int occupation = cfg.vertex_occ[pos.index(w)];
			double p_continue = 1 - std::exp(-u * occupation);
			if (occupation < 1 || uniform(rng) > p_continue)
				break;
			
			int found_count = 0;
			std::array<int, 5> found_directions;
			auto trimers = geom.get_bonds(pos, w, h);

			for (int i = 0; i < (int) trimers.size(); i++)
				if (cfg.bond_occ[trimers[i].index(w)])
					found_directions[found_count++] = i;

			ASSERT(found_count == occupation);
			ASSERT(found_count < (int) trimers.size());

			int new_dir = found_directions[rng() % found_count];
			const auto& chosen_trimer = trimers[new_dir];

			worm_flip(pos, moving, chosen_trimer, new_dir);
		}

		ASSERT(!cfg.bond_occ[moving.index(w)]);
		cfg.bond_occ[moving.index(w)] = true;
		increase_j4_bonds(moving);

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
		BondPos seed;
		while (true)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (cfg.bond_occ[candidate])
			{
				seed = BondPos::from_index(candidate, w);
				cfg.bond_occ[candidate] = false;
				break;
			}
		}
		BondPos dest;
		while (true)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			if (!cfg.bond_occ[candidate])
			{
				dest = BondPos::from_index(candidate, w);
				break;
			}
		}

		decrease_j4_bonds(seed);
		increase_j4_bonds(dest);

		int dcluster = 0;
		for (const auto& vtx : geom.get_vertices(seed, w, h))
		{
			auto& count = cfg.vertex_occ[vtx.index(w)];
			// difference due to removing one
			// (x-1)(x-2)/2 - x(x-1)/2 = 1 - x
			dcluster += 1 - count;
			ASSERT(count > 0);
			count--;
		}
		for (const auto& vtx : geom.get_vertices(dest, w, h))
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

	template <typename Rng> void reconfigure_brick_wall(Rng& rng, int vacancies = 0)
	{
		if (w != h || w % 6)
			throw "";

		std::fill(cfg.bond_occ.begin(), cfg.bond_occ.end(), false);
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

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

		BondPos cur_row_start = BondPos(0, 0, 0);

		// offset row
		if (rng() % 2)
		{
			if (orientation == 2)
				cur_row_start = BondPos(1, 0, 0);
			else
				cur_row_start = BondPos(0, 1, 0);
		}

		int n = 0;
		for (int row = 0; row < h / 2; row++)
		{
			uint offset = phase_conf[row] + row + 3 * (int)(rng() % w);

			for (int col = 0; col < 2 * w / 3; col++)
			{
				BondPos cur_pos;
				switch (orientation)
				{
				case 0:
					cur_pos = BondPos(cur_row_start.x + offset / 2, cur_row_start.y, offset % 2);
					break;
				case 1:
					cur_pos =
						BondPos(cur_row_start.x - offset / 2 - offset % 2, cur_row_start.y + offset / 2, offset % 2);
					break;
				case 2:
					cur_pos = BondPos(cur_row_start.x, cur_row_start.y - (offset + 1) / 2, offset % 2);
					break;
				}

				offset += 3;

				if (n++ >= vacancies)
				{
					cfg.bond_occ[cur_pos.canonical(w, h).index(w)] = true;
					for (auto& vtx : geom.get_vertices(cur_pos, w, h))
						cfg.vertex_occ[vtx.index(w)]++;
				}
			}

			switch (orientation)
			{
			case 0:
				cur_row_start = BondPos(cur_row_start.x, cur_row_start.y + 2, 0);
				break;
			case 1:
				cur_row_start = BondPos(cur_row_start.x - 2, cur_row_start.y, 0);
				break;
			case 2:
				cur_row_start = BondPos(cur_row_start.x + 2, cur_row_start.y - 2, 0);
				break;
			}
		}

		cfg.j4_total = -1;
		cfg.clusters_total = 0;
	}

	template <typename Rng> void load_from(std::string file, Rng& rng, int vacancies, int line_number=-1)
	{
		std::fill(cfg.bond_occ.begin(), cfg.bond_occ.end(), false);
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		std::ifstream ifs(file);
		if (ifs.fail())
			throw "random file not found";

		std::string line;
		std::vector<std::string> lines;
		std::vector<BondPos> positions;
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

			positions.push_back(BondPos(nums[0], nums[1], nums[2]));
		}

		if (positions.size() != (uint)(w * h / 3))
			throw "file loading failed";

		std::shuffle(positions.begin(), positions.end(), rng);
		for (uint i = vacancies; i < positions.size(); i++)
		{
			const auto& pos = positions[i];
			cfg.bond_occ[pos.index(w)] = true;
			for (auto& vtx : geom.get_vertices(pos, w, h))
				cfg.vertex_occ[vtx.index(w)]++;
		}

		cfg.j4_total = -1;
		cfg.clusters_total = -1;
	}

	template <typename Rng> void reconfigure_root3(Rng& rng, int vacancies = 0)
	{
		if (w != h || w % 3)
			throw "";

		std::fill(cfg.bond_occ.begin(), cfg.bond_occ.end(), false);
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		int offset = (int)(rng() % 6);

		int n = 0;
		for (int i = 0; i < w; i += 3)
			for (int j = 0; j < w; j += 3)
				for (int s = 0; s < 3; s++)
					if (n++ >= vacancies)
					{
						BondPos pos;
						pos = BondPos(i + offset / 2 + s, j + s, offset % 2);
						cfg.bond_occ[pos.canonical(w, h).index(w)] = true;
						for (auto& vtx : geom.get_vertices(pos, w, h))
							cfg.vertex_occ[vtx.index(w)]++;
					}

		cfg.j4_total = 0;
		cfg.clusters_total = 0;
	}
};

namespace std
{
template <> struct hash<Sample>
{
	std::size_t operator()(const Sample& x) const
	{
		std::size_t seed = 0;
		for (size_t i = 0; i < x.cfg.bond_occ.size(); i++)
		{
			if (x.cfg.bond_occ[i])
				seed ^= std::hash<BondPos>()(BondPos::from_index((uint32_t)i, x.w))
					+ 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
}

struct SampleDimer
{
  private:
	std::vector<BondPos> _pocket, _Abar;
	std::unordered_set<BondPos> _candidates;

  public:
	int32_t w, h;
	DimerGeometryProvider geom;

	struct Configuration
	{
		std::vector<bool> bond_occ;
		std::vector<uint8_t> vertex_occ;
	};

	Configuration cfg;

	SampleDimer() : w(0), h(0) {}

	SampleDimer(int32_t w, int32_t h) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 2, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h);
	}

	SampleDimer(int32_t w, int32_t h, const std::vector<BondPos>& dimers) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 2, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h);
		for (auto& i : dimers)
			cfg.bond_occ[i.index(w)] = true;

		regenerate_occupation();
	}

	void regenerate_occupation()
	{
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		for (uint i = 0; i < cfg.bond_occ.size(); i++)
			if (cfg.bond_occ[i])
			{
				auto tri = BondPos::from_index(i, w);
				for (auto& vtx : geom.get_vertices(tri, w, h))
					cfg.vertex_occ[vtx.index(w)]++;
			}
	}

	int calculate_winding_number(BondPos start) const
	{
		BondPos cpos = start;
		int tot = 0;
		int sign = 1;

		while (true)
		{
			if (cfg.bond_occ[cpos.index(w)])
				tot += sign;

			switch (start.s)
			{
			case 0:
				cpos = BondPos(cpos.x, cpos.y + 1, 0);
				break;
			case 1:
				cpos = BondPos(cpos.x + 1, cpos.y, 1);
				break;
			}

			sign *= -1;
			cpos = cpos.canonical(w, h);

			if (cpos == start)
				break;
		}
		return tot;
	}

	std::vector<int> calculate_winding_numbers() const
	{
		std::vector<int> ret;

		for (int dir = 0; dir < 2; dir++)
			ret.push_back(calculate_winding_number(BondPos(0, 0, dir)));

		return ret;
	}

	BondPos dimer_reflect(BondPos pos, VertexPos center, int dir)
	{
		auto p = pos.center_at(center, w, h);
		int32_t cx = center.x, cy = center.y;
		BondPos np;
		switch (dir)
		{
		case 0:
			np = !p.s ? BondPos(cx + p.x, cy - p.y, 0) : BondPos(cx + p.x, cy - p.y - 1, 1);
			break;
		case 1:
			np = BondPos(cx + p.y, cy + p.x, 1 - p.s);
			break;
		case 2:
			np = !p.s ? BondPos(cx - p.x - 1, cy + p.y, 0) : BondPos(cx - p.x, cy + p.y, 1);
			break;
		case 3:
			np = !p.s ? BondPos(cx - p.y, cy - p.x - 1, 1) : BondPos(cx - p.y - 1, cy - p.x, 0);
			break;
		}

		return np.canonical(w, h);
	}

	template <typename Rng> int pocket_move(Rng& rng)
	{
		VertexPos symc;
		int syma;

		BondPos seed(0, 0, -1);
		while (seed.s == -1)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			symc = VertexPos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % 4);

			if (cfg.bond_occ[candidate])
			{
				seed = BondPos::from_index(candidate, w);
				if (cfg.bond_occ[dimer_reflect(seed, symc, syma).index(w)])
					seed = BondPos(0, 0, -1);
			}
		}

		_Abar.clear();
		_pocket.clear();
		_pocket.push_back(seed);

		cfg.bond_occ[seed.index(w)] = false;
		for (auto& i : geom.get_vertices(seed, w, h))
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

			auto moved = dimer_reflect(el, symc, syma);
			_Abar.push_back(moved);

			_candidates.clear();

			// add target overlaps to candidates
			for (auto& i : geom.get_vertices(moved, w, h))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i, w, h))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates.insert(trimer_pos);

			for (const auto& pos : _candidates)
			{
				_pocket.push_back(pos);
				cfg.bond_occ[pos.index(w)] = false;
				for (auto& vtx : geom.get_vertices(pos, w, h))
				{
					ASSERT(cfg.vertex_occ[vtx.index(w)] > 0);
					cfg.vertex_occ[vtx.index(w)]--;
				}
			}
		}

		for (auto& i : _Abar)
		{
			ASSERT(cfg.bond_occ[i.index(w)] == false)
			cfg.bond_occ[i.index(w)] = true;

			for (auto& vtx : geom.get_vertices(i, w, h))
				cfg.vertex_occ[vtx.index(w)]++;
		}

#ifdef TEST
		auto occupations = cfg.vertex_occ;
		regenerate_occupation();
		ASSERT(occupations == cfg.vertex_occ)
#endif

		return length;
	}
};

struct VertexPos2
{
	int32_t x, y;
	int8_t s;

	VertexPos2() : x(0), y(0), s(0) {}
	VertexPos2(int32_t x, int32_t y, int8_t s) : x(x), y(y), s(s) {}

	uint32_t index(int w) const { return (x + y * w) * 2 + s; }
	static VertexPos2 from_index(uint32_t index, int w)
	{
		uint32_t half = index / 2;
		return VertexPos2(half % w, half / w, index % 2);
	}

	bool operator==(const VertexPos2& other) const { return x == other.x && y == other.y && s == other.s; }
};

struct BondPos3
{
	int32_t x, y;
	int8_t s;

	BondPos3() : x(0), y(0), s(0) {}
	BondPos3(int x, int y, int s) : x(x), y(y), s((int8_t)s) {}

	uint32_t index(int w) const { return (x + y * w) * 3 + s; }
	static BondPos3 from_index(uint32_t index, int w)
	{
		uint32_t half = index / 3;
		return BondPos3(half % w, half / w, index % 3);
	}

	bool operator==(const BondPos3& other) const { return x == other.x && y == other.y && s == other.s; }
};

std::ostream& operator<<(std::ostream& o, const VertexPos2& v) { return o << "(" << v.x << "," << v.y << "," << (int)v.s << ")"; }
std::ostream& operator<<(std::ostream& o, const BondPos3& v)
{
	return o << "(" << v.x << "," << v.y << "," << (int)v.s << ")";
}

namespace std
{
template <> struct hash<VertexPos2>
{
	std::size_t operator()(const VertexPos2& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
template <> struct hash<BondPos3>
{
	std::size_t operator()(const BondPos3& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(x.s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
} // namespace std

struct DimerHexaGeometryProvider
{
	VertexPos canonicalize(VertexPos p, int w, int h) const
	{
		while (p.y >= h)
		{
			p.x -= h;
			p.y -= h;
		}
		while (p.y < 0)
		{
			p.x += h;
			p.y += h;
		}
		p.x = pmod(p.x, w);

		return p;
	}

	BondPos3 canonicalize(BondPos3 p, int w, int h) const
	{
		auto canon = canonicalize(VertexPos(p.x, p.y), w, h);
		return BondPos3(canon.x, canon.y, p.s);
	}

	VertexPos2 canonicalize(VertexPos2 p, int w, int h) const
	{
		auto canon = canonicalize(VertexPos(p.x, p.y), w, h);
		return VertexPos2(canon.x, canon.y, p.s);
	}

	std::array<VertexPos2, 2> get_vertices(BondPos3 pos, int w, int h) const
	{
		std::array<VertexPos2, 2> r;
		r[0] = {pos.x, pos.y, 0};

		if (pos.s == 0)
			r[1] = {pos.x, pos.y, 1};
		else if (pos.s == 1)
			r[1] = {pos.x-1, pos.y, 1};
		else
			r[1] = {pos.x, pos.y-1, 1};

		r[0] = canonicalize(r[0], w, h);
		r[1] = canonicalize(r[1], w, h);

		return r;
	}

	std::array<std::array<BondPos3, 3>, 2> vertex_dimers = {
		std::array<BondPos3, 3>{ BondPos3{0, 0, 0}, {0, 0, 1}, {0, 0, 2} },
		std::array<BondPos3, 3>{ BondPos3{0, 0, 0}, {0, 1, 2}, {1, 0, 1} },
	};

	std::array<BondPos3, 3> get_bonds(VertexPos2 pos, int w, int h) const
	{
		std::array<BondPos3, 3> r;

		for (int i = 0; i < 3; i++)
		{
			r[i] = vertex_dimers[pos.s][i];
			r[i].x += pos.x;
			r[i].y += pos.y;
			r[i] = canonicalize(r[i], w, h);
		}

		return r;
	}

	VertexPos vertex_reflect(VertexPos pos, VertexPos center, int dir, int w, int h)
	{
		pos.x -= center.x;
		pos.y -= center.y;

		VertexPos np;

		switch (dir)
		{
		default:
		case 0:
			np = VertexPos(pos.x+pos.y, -pos.y);
			break;
		case 1:
			np = VertexPos(pos.y, pos.x);
			break;
		case 2:
			np = VertexPos(-pos.x, pos.x+pos.y);
			break;
		case 3:
			np = VertexPos(-pos.x-pos.y, pos.y);
			break;
		case 4:
			np = VertexPos(-pos.y, -pos.x);
			break;
		case 5:
			np = VertexPos(pos.x, -pos.x-pos.y);
			break;
		}

		np.x += center.x;
		np.y += center.y;
		return canonicalize(np, w, h);
	}

	std::array<std::array<BondPos3, 6>, 3> dimer_reflections = {
		std::array<BondPos3, 6>{ BondPos3{1, -1, 1}, {0, 0, 0}, {-1, 1, 2}, {-1, 0, 1}, {-1, -1, 0}, {0, -1, 2} },
		std::array<BondPos3, 6>{ BondPos3{0, -1, 0}, {0, 0, 2}, {0, 0, 1}, {-1, 0, 0}, {-1, 0, 2}, {0, -1, 1} },
		std::array<BondPos3, 6>{ BondPos3{0, 0, 2}, {0, 0, 1}, {-1, 0, 0}, {-1, 0, 2}, {0, -1, 1}, {0, -1, 0} }
	};

	BondPos3 dimer_reflect(BondPos3 pos, VertexPos center, int dir, int w, int h)
	{
		VertexPos vp = vertex_reflect(VertexPos(pos.x, pos.y), center, dir, w, h);
		BondPos3 np = BondPos3(dimer_reflections[pos.s][dir]);
		np.x += vp.x + center.x;
		np.y += vp.y + center.y;
		return canonicalize(np, w, h);
	}
};

struct SampleDimerHexa
{
  private:
	std::vector<BondPos3> _pocket, _Abar;
	std::unordered_set<BondPos3> _candidates;

  public:
	int32_t w, h;
	DimerHexaGeometryProvider geom;

	struct Configuration
	{
		std::vector<bool> bond_occ;
		std::vector<uint8_t> vertex_occ;
	};

	Configuration cfg;

	SampleDimerHexa() : w(0), h(0) {}

	SampleDimerHexa(int32_t w, int32_t h) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 3, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h * 2, 0);
	}

	SampleDimerHexa(int32_t w, int32_t h, const std::vector<BondPos3>& dimers) : w(w), h(h)
	{
		cfg.bond_occ = std::vector<bool>(w * h * 3, false);
		cfg.vertex_occ = std::vector<uint8_t>(w * h * 2, 0);

		for (auto& i : dimers)
			cfg.bond_occ[i.index(w)] = true;

		regenerate_occupation();
	}

	void regenerate_occupation()
	{
		std::fill(cfg.vertex_occ.begin(), cfg.vertex_occ.end(), 0);

		for (uint i = 0; i < cfg.bond_occ.size(); i++)
			if (cfg.bond_occ[i])
			{
				auto tri = BondPos3::from_index(i, w);
				for (auto& vtx : geom.get_vertices(tri, w, h))
					cfg.vertex_occ[vtx.index(w)]++;
			}
	}

	int calculate_winding_number(int dir) const
	{
		BondPos3 pos(0, 0, 0);
		if (dir == 0)
			pos.s = 2;
		else
			pos.s = 1;

		int tot = -w / 3;

		while (true)
		{
			if (cfg.bond_occ[pos.index(w)])
				tot++;

			if (dir == 0)
				pos.x++;
			else
				pos.y++;

			pos = geom.canonicalize(pos, w, h);

			if (pos.x == 0 && pos.y == 0)
				break;
		}

		return tot;
	}

	std::pair<int, int> calculate_topo_sector() const
	{
		int wa = calculate_winding_number(0);
		int wb = calculate_winding_number(1);

		// int ri = (wa - wb) / 3, rj = (2 * wa + wb) / 3;
		return std::make_pair(wa, wb);
	}

	template <typename Rng> int pocket_move(Rng& rng)
	{
		VertexPos symc;
		int syma;

		BondPos3 seed(0, 0, -1);
		while (seed.s == -1)
		{
			uint candidate = (uint)(rng() % cfg.bond_occ.size());
			symc = VertexPos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % 6);

			if (cfg.bond_occ[candidate])
			{
				seed = BondPos3::from_index(candidate, w);
				if (cfg.bond_occ[geom.dimer_reflect(seed, symc, syma, w, h).index(w)])
					seed.s = -1;
			}
		}

		_Abar.clear();
		_pocket.clear();
		_pocket.push_back(seed);

		cfg.bond_occ[seed.index(w)] = false;
		for (auto& i : geom.get_vertices(seed, w, h))
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

			auto moved = geom.dimer_reflect(el, symc, syma, w, h);
			_Abar.push_back(moved);

			_candidates.clear();

			// add target overlaps to candidates
			for (auto& i : geom.get_vertices(moved, w, h))
				if (cfg.vertex_occ[i.index(w)] > 0)
					for (const auto& trimer_pos : geom.get_bonds(i, w, h))
						if (cfg.bond_occ[trimer_pos.index(w)])
							_candidates.insert(trimer_pos);

			for (const auto& pos : _candidates)
			{
				_pocket.push_back(pos);
				cfg.bond_occ[pos.index(w)] = false;
				for (auto& vtx : geom.get_vertices(pos, w, h))
				{
					ASSERT(cfg.vertex_occ[vtx.index(w)] > 0);
					cfg.vertex_occ[vtx.index(w)]--;
				}
			}
		}

		for (auto& i : _Abar)
		{
			ASSERT(cfg.bond_occ[i.index(w)] == false)
			cfg.bond_occ[i.index(w)] = true;

			for (auto& vtx : geom.get_vertices(i, w, h))
				cfg.vertex_occ[vtx.index(w)]++;
		}

#ifdef TEST
		auto occupations = cfg.vertex_occ;
		regenerate_occupation();
		ASSERT(occupations == cfg.vertex_occ)
#endif

		return length;
	}
};

template <typename T> inline void write_binary(std::ostream& o, const T& t)
{
	o.write(reinterpret_cast<const char*>(&t), sizeof(t));
}

struct PTWorker
{
	const Options options;

	double temperature;
	Sample sample;
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

	double pocket_fraction = 0.08;

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
		sample = Sample(opt.domain_length, opt.domain_length);

		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		sample.initialize(opt, rng);
		pocket_fraction = options.accumulator_interval / 100.;

		if (output)
			enable_io();

#ifdef ERGOTEST
		std::stringstream fname;
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << pocket_fraction << ".csv";
		ofunique = std::ofstream(fname.str());
		fname.str("");
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << pocket_fraction << "_thist.dat";
		oftrimerhist = std::ofstream(fname.str(), std::ios::binary);
		fname.str("");
		fname << "new-data/worm-timing/" << opt.domain_length << "_" << pocket_fraction << "_topohist.dat";
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
				sample.reconfigure_brick_wall(rng, false);
			else if (options.idealrt3)
				sample.reconfigure_root3(rng);
			else if (options.metropolis)
			{
				sample.calculate_energy();
				Sample::Configuration copy = sample.cfg;

				sample.metropolis_move(rng);
				sample.calculate_energy();

				double d_energy =
					(sample.cfg.j4_total - copy.j4_total) * j4 + (sample.cfg.clusters_total - copy.clusters_total) * u;
				if (uniform(rng) <= 1 - std::exp(-d_energy))
					std::swap(copy, sample.cfg);
			}
			else
			{
				if (uniform(rng) < pocket_fraction)
					sample.pocket_move(rng, u, j4);
				else
					sample.worm_move(rng, u);
			}

#ifdef ERGOTEST
			if (sample.w <= 15)
				seen.insert(sample.cfg.bond_occ);
			write_binary<float>(oftrimerhist, (float)sample.trimer_correlation(BondPos(1, 0, 1)));

			auto sector = sample.calculate_topo_sector();
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
			Sample::Observables obs;
			double energy = 0;

			if (options.out_monomono)
				sample.record_partial_monomer_correlations(amonomono, rng, 32);
			if (options.out_clustercount)
				sample.record_cluster_count(aclustercount);
			if (options.out_monodi)
				sample.record_dimer_monomer_correlations(amonodi, rng, 10);
			if (options.out_tritri)
				sample.record_partial_trimer_correlations(atritri, rng, 20. / (sample.w * sample.h / 3));
			if (options.out_energy)
				energy = sample.record_energy(aenergy, options.u, options.j4);
			if (options.out_order)
				obs = sample.record_order_parameters(aorder);

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
						ofconf << BondPos::from_index(j, sample.w) << " ";
				ofconf << std::endl;
				previous_position_output = total_steps;
			}

			if (options.out_histogram)
			{
				write_binary<float>(ofhistogram, (float)energy);
				write_binary<float>(ofhistogram, (float)obs.structure_factor_K);
				write_binary<int32_t>(ofhistogram, obs.sublattice_polarization);
				ofhistogram.flush();
			}

			if (options.out_winding_histogram)
			{
				write_binary<float>(ofsector, (float)energy);
				auto sector = sample.calculate_topo_sector();
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

	Sample sample;
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
		sample = Sample(opt.domain_length, opt.domain_length);

		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		factor = options.multicanonical_init_factor;

		sample.initialize(opt, rng);
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
			Sample::Configuration copy = sample.cfg;
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

	if (optstring.find('D') != std::string::npos)
	{
		std::minstd_rand rng;
		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		std::vector<BondPos> positions;
		for (int i = 0; i < options.domain_length; i += 2)
			for (int j = 0; j < options.domain_length; j += 1)
				positions.push_back(BondPos(i, j, 0));

		SampleDimer sample(options.domain_length, options.domain_length, positions);

		auto path = std::filesystem::path("new-data") / options.directory;
		std::filesystem::create_directories(path);
		std::ofstream ofhist(path / "winding-histogram.dat", std::ios::binary);
		std::ofstream ofconf(path / "positions.dat");

		int nstep = 0;
		int prev_conf = 0;
		while (nstep < options.total_steps)
		{
			for (int s = 0; s < options.decorr_interval; s++)
				sample.pocket_move(rng);
			nstep += options.decorr_interval;

			auto windings = sample.calculate_winding_numbers();
			for (int i = 0; i < 2; i++)
				write_binary<int16_t>(ofhist, (int16_t)windings[i]);
			ofhist.flush();

			if (nstep >= prev_conf + options.position_interval)
			{
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
						ofconf << BondPos::from_index(j, sample.w) << " ";
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

		std::vector<BondPos3> positions;
		for (int i = 0; i < options.domain_length; i += 1)
			for (int j = 0; j < options.accumulator_interval; j += 1)
				// positions.push_back(BondPos3(i, j, 0));
				positions.push_back(BondPos3(i, j, pmod(-i+j, 3)));

		SampleDimerHexa sample(options.domain_length, options.accumulator_interval, positions);

		auto path = std::filesystem::path("new-data") / options.directory;
		std::filesystem::create_directories(path);
		std::ofstream ofhist(path / "winding-histogram.dat", std::ios::binary);
		std::ofstream ofconf(path / "positions.dat");

		int nstep = 0;
		int prev_conf = 0;
		while (nstep < options.total_steps)
		{
			for (int s = 0; s < options.decorr_interval; s++)
				sample.pocket_move(rng);
			nstep += options.decorr_interval;

			auto windings = sample.calculate_topo_sector();
			write_binary<int16_t>(ofhist, (int16_t)windings.first);
			write_binary<int16_t>(ofhist, (int16_t)windings.second);
			ofhist.flush();

			if (nstep >= prev_conf + options.position_interval)
			{
				for (uint j = 0; j < sample.cfg.bond_occ.size(); j++)
					if (sample.cfg.bond_occ[j])
						ofconf << BondPos3::from_index(j, sample.w) << " ";
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
	Sample sample(6, 6);
	std::minstd_rand rng;
	sample.reconfigure_brick_wall(rng);

	sample.calculate_energy();
	ASSERT(sample.cfg.clusters_total == 0);

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.pocket_move(rng, infinity, 0);
		sample.calculate_energy();
		ASSERT(sample.cfg.clusters_total == 0);

		int ntri = 0;
		for (auto j : sample.cfg.bond_occ)
			if (j)
				ntri++;
		ASSERT(ntri == 6 * 6 / 3);
	}

	std::cout << "pocket test finished with avg length " << total / 300. << std::endl;

	total = 0;
	for (int i = 0; i < 300; i++)
		total += sample.pocket_move(rng, 1, 0.01);

	std::cout << "pocket test finished with avg length " << total / 300. << std::endl;
}

void test_worm1()
{
	Sample sample(6, 6);
	std::minstd_rand rng;
	sample.reconfigure_brick_wall(rng);

	sample.calculate_energy();
	ASSERT(sample.cfg.clusters_total == 0);

	double total = 0;
	for (int i = 0; i < 300; i++)
	{
		total += sample.worm_move(rng, infinity);
		sample.calculate_energy();
		ASSERT(sample.cfg.clusters_total == 0);

		int ntri = 0;
		for (auto j : sample.cfg.bond_occ)
			if (j)
				ntri++;
		ASSERT(ntri == 6 * 6 / 3);
	}

	std::cout << "worm test finished with avg length " << total / 300. << std::endl;

	total = 0;
	for (int i = 0; i < 300; i++)
		total += sample.worm_move(rng, 1);

	std::cout << "worm test finished with avg length " << total / 300. << std::endl;
}

void test_brickwall()
{
	Sample sample(48, 48);
	std::minstd_rand rng;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 20; j++)
			sample.reconfigure_brick_wall(rng);

		sample.calculate_energy();
		TEST_ASSERT2(sample.cfg.j4_total == 48 * 48 / 2, sample.cfg.j4_total)
	}
}

void test_symmetry()
{
	int w = 12;

	for (int d = 0; d < 17; d++)
	{
		std::vector<bool> occupancies(w * w * 2);
		for (int i = 0; i < w; i++)
			for (int j = 0; j < w; j++)
				for (int s = 0; s < 2; s++)
					occupancies[BondPos(i, j, s).apply_symmetry(VertexPos(1, 2), d, w, w).index(w)] = true;

		TEST_ASSERT(std::find(occupancies.begin(), occupancies.end(), false) == occupancies.end());
	}
}

void test_symmetry_hexa()
{
	DimerHexaGeometryProvider geom;
	int w = 12;
	
	for (int d = 0; d < 6; d++)
	{
		std::vector<bool> occupancies(w * w * 3);
		for (int i = 0; i < w; i++)
			for (int j = 0; j < w; j++)
				for (int s = 0; s < 3; s++)
					occupancies[geom.dimer_reflect(BondPos3(i, j, s), VertexPos(1, 2), d, w, w).index(w)] = true;

		TEST_ASSERT(std::find(occupancies.begin(), occupancies.end(), false) == occupancies.end());
	}
}

#ifndef TEST
int main(int argc, char** argv) { sim(argc, argv); }
#else
int main()
{
	test_pocket1();
	test_worm1();
	test_brickwall();
	test_symmetry();
	test_symmetry_hexa();
	test_optimizer();
}
#endif
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
void assert_fail() { exit(0); }
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
	}                                                                                                                  \
	else                                                                                                               \
	{                                                                                                                  \
		std::cout << "test passed at " << __LINE__ << #x << std::endl;                                                 \
	}
#define TEST_ASSERT2(x, y)                                                                                             \
	if (!(x))                                                                                                          \
	{                                                                                                                  \
		std::cout << "test failed at " << __LINE__ << #x << "; " << (y) << std::endl;                                  \
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

int32_t pmod(int32_t a, int32_t b) { return (a % b + b) % b; }

const double infinity = std::numeric_limits<double>::infinity();
using complex = std::complex<double>;

struct MonomerPos
{
	int32_t x, y;

	MonomerPos() : x(0), y(0) {}
	MonomerPos(int32_t x, int32_t y) : x(x), y(y) {}

	MonomerPos rotate(int dir)
	{
		switch (pmod(dir, 6))
		{
		default:
		case 0:
			return MonomerPos(x, y);
		case 1:
			return MonomerPos(-y, x + y);
		case 2:
			return MonomerPos(-x - y, x);
		case 3:
			return MonomerPos(-x, -y);
		case 4:
			return MonomerPos(y, -x - y);
		case 5:
			return MonomerPos(x + y, -x);
		}
	}

	MonomerPos canonical(int32_t w, int32_t h) const { return MonomerPos(pmod(x, w), pmod(y, h)); }
	MonomerPos center_at(MonomerPos pos, int32_t w, int32_t h) const
	{
		return MonomerPos(pmod(x - pos.x + w / 2, w) - w / 2, pmod(y - pos.y + h / 2, h) - h / 2);
	}

	uint32_t index(int w) const { return x + y * w; }
	static MonomerPos from_index(uint32_t index, int w) { return MonomerPos(index % w, index / w); }

	MonomerPos operator-() const { return MonomerPos(-x, -y); }
	MonomerPos operator-(const MonomerPos& other) const { return MonomerPos(x - other.x, y - other.y); }
	MonomerPos operator+(const MonomerPos& other) const { return MonomerPos(x + other.x, y + other.y); }
	bool operator==(const MonomerPos& other) const { return x == other.x && y == other.y; }
};

struct TrimerPos;

struct Cluster
{
	uint8_t occupations = 0;
	static Cluster relative(int dx, int dy, int s)
	{
		if (dx != 0)
			dx = 1;
		if (dy != 0)
			dy = 1;

		return Cluster{(uint8_t)(1 << (dx * 4 + dy * 2 + s))};
	}

	Cluster() {}
	Cluster(uint8_t occupations) : occupations(occupations) {}

	bool operator==(const Cluster& other) const { return occupations == other.occupations; }
};

struct TrimerPos
{
	int32_t x, y;
	int8_t s;

	TrimerPos() : x(0), y(0), s(0) {}
	TrimerPos(int x, int y, int s) : x(x), y(y), s((int8_t)s) {}

	MonomerPos mono_pos() const { return MonomerPos(x, y); }

	std::array<MonomerPos, 3> get_clusters(int32_t w, int32_t h) const
	{
		std::array<MonomerPos, 3> r;
		r[0] = {x + 1, y};
		r[1] = {x, y + 1};
		if (s == 0)
			r[2] = {x, y};
		else
			r[2] = {x + 1, y + 1};

		for (auto& i : r)
			i = i.canonical(w, h);

		return r;
	}

	std::array<std::pair<MonomerPos, Cluster>, 3> get_clusters_wrel(int32_t w, int32_t h) const
	{
		std::array<std::pair<MonomerPos, Cluster>, 3> r;
		if (s == 0)
		{
			r[0] = {{x + 1, y}, Cluster::relative(-1, 0, 0)};
			r[1] = {{x, y + 1}, Cluster::relative(0, -1, 0)};
			r[2] = {{x, y}, Cluster::relative(0, 0, 0)};
		}
		else
		{
			r[0] = {{x + 1, y}, Cluster::relative(-1, 0, 1)};
			r[1] = {{x, y + 1}, Cluster::relative(0, -1, 1)};
			r[2] = {{x + 1, y + 1}, Cluster::relative(-1, -1, 1)};
		}

		for (auto& i : r)
			i.first = i.first.canonical(w, h);

		return r;
	}

	TrimerPos reflect(MonomerPos center, int dir, int w, int h) const
	{
		auto p = center_at(center, w, h);
		int32_t cx = center.x, cy = center.y;
		TrimerPos np;
		switch (dir)
		{
		case 0:
			np = !s ? TrimerPos(cx + p.x + p.y, cy - p.y - 1, 1) : TrimerPos(cx + p.x + p.y + 1, cy - p.y - 1, 0);
			break;
		case 1:
			np = !s ? TrimerPos(cx - p.x - 1, cy + p.y + p.x, 1) : TrimerPos(cx - p.x - 1, cy + p.y + p.x + 1, 0);
			break;
		case 2:
			np = !s ? TrimerPos(cx - p.y - 1, cy - p.x - 1, 1) : TrimerPos(cx - p.y - 1, cy - p.x - 1, 0);
			break;
		case 3:
			np = !s ? TrimerPos(cx - p.x - p.y - 1, cy + p.y, 0) : TrimerPos(cx - p.x - p.y - 2, cy + p.y, 1);
			break;
		case 4:
			np = !s ? TrimerPos(cx + p.x, cy - p.y - p.x - 1, 0) : TrimerPos(cx + p.x, cy - p.y - p.x - 2, 1);
			break;
		case 5:
			np = !s ? TrimerPos(cx + p.y, cy + p.x, 0) : TrimerPos(cx + p.y, cy + p.x, 1);
			break;
		default:
			np = TrimerPos(x, y, s);
			break;
		}

		return np.canonical(w, h);
	}

	TrimerPos canonical(int32_t w, int32_t h) const { return TrimerPos(pmod(x, w), pmod(y, h), s); }
	TrimerPos center_at(MonomerPos pos, int32_t w, int32_t h) const
	{
		return TrimerPos(pmod(x - pos.x + w / 2, w) - w / 2, pmod(y - pos.y + h / 2, h) - h / 2, s);
	}

	uint32_t index(int w) const { return (x + y * w) * 2 + s; }
	static TrimerPos from_index(uint32_t index, int w)
	{
		uint32_t half = index / 2;
		return TrimerPos(half % w, half / w, index % 2);
	}

	bool operator==(const TrimerPos& other) const { return x == other.x && y == other.y && s == other.s; }
};

struct InteractionCount
{
	int u = 0;
	int j4 = 0;

	bool operator==(const InteractionCount& other) const { return j4 == other.j4 && u == other.u; }
};

namespace std
{
template <> struct hash<MonomerPos>
{
	std::size_t operator()(const MonomerPos& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
template <> struct hash<TrimerPos>
{
	std::size_t operator()(const TrimerPos& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(x.s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
template <> struct hash<InteractionCount>
{
	std::size_t operator()(const InteractionCount& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.u);
		seed ^= std::hash<int32_t>()(x.j4) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
} // namespace std

std::ostream& operator<<(std::ostream& o, const MonomerPos& v) { return o << "(" << v.x << "," << v.y << ")"; }
std::ostream& operator<<(std::ostream& o, const TrimerPos& v)
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

struct Sample
{
  private:
	std::vector<TrimerPos> _pocket, _Abar;
	std::vector<std::pair<MonomerPos, Cluster>> _Abar_entries;
	std::unordered_map<TrimerPos, InteractionCount> _candidates;
	std::unordered_map<int, std::vector<complex>> _ft_contributions;

  public:
	static const std::array<std::array<MonomerPos, 6>, 2> j4_neighbor_list;
	static const std::array<MonomerPos, 3> j6_neighbor_list;

	int32_t w, h;
	std::vector<bool> trimer_occupations;
	std::vector<Cluster> vertex_occupations;
	int j4_total, clusters_total;

	Sample() : w(0), h(0)
	{
		j4_total = 0;
		clusters_total = 0;
	}

	Sample(int32_t w, int32_t h) : w(w), h(h)
	{
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		vertex_occupations = std::vector<Cluster>(w * h);
		j4_total = 0;
		clusters_total = 0;
	}

	Sample(int32_t w, int32_t h, const std::vector<TrimerPos>& trimers) : w(w), h(h)
	{
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		for (auto& i : trimers)
			trimer_occupations[i.index(w)] = true;

		j4_total = -1;
		clusters_total = -1;

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
		vertex_occupations = std::vector<Cluster>(w * h);

		for (uint i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
			{
				auto tri = TrimerPos::from_index(i, w);

				for (auto& [occ, rel] : tri.get_clusters_wrel(w, h))
					vertex_occupations[occ.index(w)].occupations |= rel.occupations;
			}
		}
	}

	void calculate_energy()
	{
		if (clusters_total >= 0 && j4_total >= 0)
			return;

		clusters_total = 0;
		j4_total = 0;

#ifdef TEST
		int total_occ = 0;
		for (const auto& occ : vertex_occupations)
		{
			auto popc = __builtin_popcount(occ.occupations);
			total_occ += popc;
		}
		for (auto i : trimer_occupations)
		{
			if (i)
				total_occ -= 3;
		}
		ASSERT(total_occ == 0);
#endif

		for (const auto& occ : vertex_occupations)
		{
			auto popc = __builtin_popcount(occ.occupations);
			clusters_total += popc * (popc - 1) / 2;
		}

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				if (trimer_occupations[TrimerPos(x, y, 0).index(w)])
					for (auto& rel : j4_neighbor_list[0])
						if (trimer_occupations[TrimerPos(x + rel.x, y + rel.y, 1).canonical(w, h).index(w)])
							j4_total++;
			}
		}
	}

	double trimer_correlation(TrimerPos d) const
	{
		int total = 0;
		int found = 0;

		for (uint i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
			{
				TrimerPos p1 = TrimerPos::from_index(i, w);
				TrimerPos n;

				if (p1.s == 1)
				{
					n = TrimerPos(p1.x + d.x, p1.y + d.y, p1.s).reflect(MonomerPos(0, 0), 2, w, h);
					n.s = d.s;
				}
				else
				{
					n = TrimerPos(p1.x + d.x, p1.y + d.y, d.s);
				}

				if (trimer_occupations[n.canonical(w, h).index(w)])
					found++;
				total++;
			}
		}

		return (double)found / total;
	}

	template <typename Rng>
	void record_dimer_monomer_correlations(Accumulator<double>& a, Rng& rng, int max_dimers) const
	{
		std::vector<double> vals(w * h);

		std::vector<std::pair<MonomerPos, int>> dimers;
		std::unordered_set<MonomerPos> isolated_monos;
		std::unordered_set<MonomerPos> connected_monos;

		const std::array<MonomerPos, 3> neighbors = {MonomerPos{1, 0}, {0, 1}, {-1, 1}};

		for (uint i = 0; i < vertex_occupations.size(); i++)
		{
			if (vertex_occupations[i].occupations != 0)
				continue;

			bool connected = false;
			auto pos = MonomerPos::from_index(i, w);
			for (int r = 0; r < 3; r++)
			{
				auto neighbor = (pos + neighbors[r]).canonical(w, h);
				if (vertex_occupations[neighbor.index(w)].occupations == 0)
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

	void record_trimer_correlations(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h * 2);
		for (uint i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
				for (uint j = i + 1; j < trimer_occupations.size(); j++)
					if (trimer_occupations[j])
					{
						auto p1 = TrimerPos::from_index(i, w);
						auto p2 = TrimerPos::from_index(j, w);

						auto d = p1.s == 0 ? TrimerPos(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : TrimerPos(p1.x - p2.x, p1.y - p2.y, 1 - p2.s);

						vals[d.canonical(w, h).index(w)]++;
					}
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

		for (int i = 0; i < (int)trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
			{
				auto pos = TrimerPos::from_index(i, w);
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
	void record_partial_trimer_correlations(Accumulator<double>& a, Rng& rng, double fraction) const
	{
		std::vector<double> vals(w * h * 2);
		std::uniform_real_distribution<> uniform(0., 1.);

		int seen = 0;
		for (uint i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i] && uniform(rng) <= fraction)
			{
				seen++;
				for (uint j = 0; j < trimer_occupations.size(); j++)
					if (trimer_occupations[j])
					{
						if (j == i)
							continue;

						auto p1 = TrimerPos::from_index(i, w);
						auto p2 = TrimerPos::from_index(j, w);
						if (j > i)
							std::swap(p1, p2);

						auto d = p1.s == 0 ? TrimerPos(p2.x - p1.x, p2.y - p1.y, p2.s)
										   : TrimerPos(p1.x - p2.x, p1.y - p2.y, 1 - p2.s);

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

	template <typename Rng>
	void record_partial_monomer_correlations(Accumulator<double>& a, Rng& rng, int max_count) const
	{
		std::vector<double> vals(w * h);
		std::vector<MonomerPos> pos;

		for (uint i = 0; i < vertex_occupations.size(); i++)
			if (vertex_occupations[i].occupations == 0)
				pos.push_back(MonomerPos::from_index(i, w));

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
		std::vector<MonomerPos> pos;

		for (uint i = 0; i < vertex_occupations.size(); i++)
			if (vertex_occupations[i].occupations == 0)
				pos.push_back(MonomerPos::from_index(i, w));

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

		for (uint i = 0; i < vertex_occupations.size(); i++)
			vals[__builtin_popcount(vertex_occupations[i].occupations)]++;

		a.record(vals);
	}

	double record_energy(Accumulator<double>& a, double u, double j4)
	{
		std::vector<double> vals(5);
		calculate_energy();

		vals[0] = clusters_total;
		vals[1] = j4_total;

		if (u < infinity)
			vals[2] = clusters_total * u + j4_total * j4;
		else
			vals[2] = j4_total * j4;

		vals[3] = vals[2] * vals[2];
		vals[4] = vals[3] * vals[3];

		a.record(vals);

		return vals[2];
	}

	int calculate_winding_number(TrimerPos start, int dir) const
	{
		TrimerPos cpos = start;
		int tot = -(w + h) / 6;

		while (true)
		{
			if (trimer_occupations[cpos.index(w)])
				tot++;

			switch (dir % 3)
			{
			case 0:
				if (cpos.s == 0)
					cpos = TrimerPos(cpos.x - 1, cpos.y + 1, 1);
				else
					cpos = TrimerPos(cpos.x, cpos.y + 1, 0);
				break;
			case 1:
				if (cpos.s == 0)
					cpos = TrimerPos(cpos.x, cpos.y, 1);
				else
					cpos = TrimerPos(cpos.x + 1, cpos.y + 1, 0);
				break;
			case 2:
				if (cpos.s == 0)
					cpos = TrimerPos(cpos.x + 1, cpos.y - 1, 1);
				else
					cpos = TrimerPos(cpos.x + 1, cpos.y, 0);
				break;
			}

			cpos = cpos.canonical(w, h);

			if (cpos == start)
				break;
		}
		return tot;
	}

	std::vector<int> calculate_winding_numbers() const
	{
		std::vector<int> ret;

		for (int startx = 0; startx < 3; startx++)
			for (int dir = 0; dir < 3; dir++)
				ret.push_back(calculate_winding_number(TrimerPos(startx, 0, 0), dir));

		return ret;
	}

	using Cascader = std::function<double(const InteractionCount&, double, double)>;
	static Cascader boltzmann_cascader()
	{
		return [](const InteractionCount& interactions, double u, double j4) {
			double u_factor = interactions.u != 0 ? u * interactions.u : 0;
			double j4_factor = interactions.j4 != 0 ? j4 * interactions.j4 : 0;

			return 1 - std::exp(-(u_factor + j4_factor));
		};
	}

	template <typename Rng> void pocket_move(Rng& rng, double u, double j4, Cascader cascader)
	{
		calculate_energy();

		MonomerPos symc;
		int syma;

		TrimerPos seed(0, 0, 2);
		while (seed.s == 2)
		{
			uint candidate = (uint)(rng() % trimer_occupations.size());
			symc = MonomerPos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % 6);

			if (trimer_occupations[candidate])
			{
				seed = TrimerPos::from_index(candidate, w);
				if (trimer_occupations[seed.reflect(symc, syma, w, h).index(w)])
					seed = TrimerPos(0, 0, 2);
			}
		}

		std::uniform_real_distribution<> uniform(0., 1.);

		_Abar.clear();
		_Abar_entries.clear();
		_pocket.clear();

		_pocket.push_back(seed);

		trimer_occupations[seed.index(w)] = false;
		for (auto& [i, occ] : seed.get_clusters_wrel(w, h))
		{
			ASSERT((vertex_occupations[i.index(w)].occupations & occ.occupations) != 0)
			vertex_occupations[i.index(w)].occupations &= (uint8_t)~occ.occupations;
		}

		while (_pocket.size() > 0)
		{
			auto el = _pocket.back();
			_pocket.pop_back();

			auto moved = el.reflect(symc, syma, w, h);
			_Abar.push_back(moved);

			_candidates.clear();

			// add target overlaps to candidates
			for (const auto& [i, rel] : moved.get_clusters_wrel(w, h))
			{
				_Abar_entries.push_back(std::make_pair(i, rel));
				auto target_occ = vertex_occupations[i.index(w)].occupations;

				if (target_occ > 0 && u != 0)
				{
					for (int dx = -1; dx <= 0; dx++)
						for (int dy = -1; dy <= 0; dy++)
							for (int s = 0; s < 2; s++)
								if ((target_occ & Cluster::relative(dx, dy, s).occupations) != 0)
								{
									auto overlap = TrimerPos(i.x + dx, i.y + dy, s).canonical(w, h);
									_candidates[overlap].u++;
								}
				}
			}

			// add source overlaps to candidates
			if (u != 0 && u < infinity)
				for (auto& [i, rel] : el.get_clusters_wrel(w, h))
				{
					auto orig_occ = vertex_occupations[i.index(w)].occupations;

#if DTEST
					ASSERT(orig_occ != 0)
#endif
					for (int dx = -1; dx <= 0; dx++)
						for (int dy = -1; dy <= 0; dy++)
							for (int s = 0; s < 2; s++)
								if ((orig_occ & Cluster::relative(dx, dy, s).occupations) != 0)
								{
									auto overlap = TrimerPos(i.x + dx, i.y + dy, s).canonical(w, h);
									_candidates[overlap].u--;
								}
				}

			if (j4 != 0)
			{
				for (auto& d : j4_neighbor_list[el.s])
				{
					auto pos = TrimerPos(el.x + d.x, el.y + d.y, 1 - el.s).canonical(w, h);
					if (trimer_occupations[pos.index(w)])
						_candidates[pos].j4--;
				}

				for (auto& d : j4_neighbor_list[moved.s])
				{
					auto pos = TrimerPos(moved.x + d.x, moved.y + d.y, 1 - moved.s).canonical(w, h);
					if (trimer_occupations[pos.index(w)])
						_candidates[pos].j4++;
				}
			}

			for (auto& [pos, interactions] : _candidates)
			{
				double p_cascade = cascader(interactions, u, j4);

				if (p_cascade > 0 && (p_cascade >= 1 || uniform(rng) < p_cascade))
				{
					if (trimer_occupations[pos.reflect(symc, syma, w, h).index(w)] == false)
					{
						_pocket.push_back(pos);
						trimer_occupations[pos.index(w)] = false;
						for (auto& [cluster, rel] : pos.get_clusters_wrel(w, h))
						{
							ASSERT((vertex_occupations[cluster.index(w)].occupations & rel.occupations) != 0)
							vertex_occupations[cluster.index(w)].occupations &= (uint8_t)~rel.occupations;
						}
					}
				}
			}
		}

		for (auto& i : _Abar)
		{
			ASSERT(trimer_occupations[i.index(w)] == false)
			trimer_occupations[i.index(w)] = true;
		}

		for (auto& [vertex, occ] : _Abar_entries)
		{
			ASSERT((vertex_occupations[vertex.index(w)].occupations & occ.occupations) == 0)
			vertex_occupations[vertex.index(w)].occupations |= occ.occupations;
		}

#ifdef TEST
		auto occupations = vertex_occupations;
		regenerate_occupation();
		ASSERT(occupations == vertex_occupations)
#endif

		// mark dirty
		j4_total = -1;
		clusters_total = -1;
	}

	template <typename Rng> void metropolis_move(Rng& rng)
	{
		TrimerPos seed;
		while (true)
		{
			uint candidate = (uint)(rng() % trimer_occupations.size());
			if (trimer_occupations[candidate])
			{
				seed = TrimerPos::from_index(candidate, w);
				trimer_occupations[candidate] = false;
				break;
			}
		}
		TrimerPos dest;
		while (true)
		{
			uint candidate = (uint)(rng() % trimer_occupations.size());
			if (!trimer_occupations[candidate])
			{
				dest = TrimerPos::from_index(candidate, w);
				break;
			}
		}

		int dj4 = 0;
		for (auto& rel : j4_neighbor_list[seed.s])
			if (trimer_occupations[TrimerPos(seed.x + rel.x, seed.y + rel.y, 1 - seed.s).canonical(w, h).index(w)])
				dj4--;
		for (auto& rel : j4_neighbor_list[dest.s])
			if (trimer_occupations[TrimerPos(dest.x + rel.x, dest.y + rel.y, 1 - dest.s).canonical(w, h).index(w)])
				dj4++;

		int dcluster = 0;
		for (const auto& [occ, rel] : seed.get_clusters_wrel(w, h))
		{
			auto& popc = vertex_occupations[occ.index(w)].occupations;
			ASSERT((popc & rel.occupations) != 0);

			// difference due to removing one
			// (x-1)(x-2)/2 - x(x-1)/2 = 1 - x
			dcluster += 1 - __builtin_popcount(popc);
			popc &= (uint8_t)~rel.occupations;
		}
		for (const auto& [occ, rel] : dest.get_clusters_wrel(w, h))
		{
			auto& popc = vertex_occupations[occ.index(w)].occupations;
			ASSERT((popc & rel.occupations) == 0);

			// difference due to adding one
			// x(x+1)/2 - x(x-1)/2 = x
			dcluster += __builtin_popcount(popc);
			popc |= rel.occupations;
		}

		trimer_occupations[dest.index(w)] = true;

		j4_total += dj4;
		clusters_total += dcluster;

#ifdef TEST
		int oldj4 = j4_total;
		int oldcluster = clusters_total;
		j4_total = -1;
		clusters_total = -1;
		calculate_energy();
		ASSERT(j4_total == oldj4)
		ASSERT(clusters_total == oldcluster)

		auto occupations = vertex_occupations;
		regenerate_occupation();
		ASSERT(occupations == vertex_occupations)
#endif
	}

	template <typename Rng> void reconfigure_brick_wall(Rng& rng, int vacancies = 0)
	{
		if (w != h || w % 6)
			throw "";

		std::fill(trimer_occupations.begin(), trimer_occupations.end(), false);
		std::fill(vertex_occupations.begin(), vertex_occupations.end(), Cluster());

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

		TrimerPos cur_row_start = TrimerPos(0, 0, 0);

		// offset row
		if (rng() % 2)
		{
			if (orientation == 2)
				cur_row_start = TrimerPos(1, 0, 0);
			else
				cur_row_start = TrimerPos(0, 1, 0);
		}

		int n = 0;
		for (int row = 0; row < h / 2; row++)
		{
			uint offset = phase_conf[row] + row + 3 * (int)(rng() % w);

			for (int col = 0; col < 2 * w / 3; col++)
			{
				TrimerPos cur_pos;
				switch (orientation)
				{
				case 0:
					cur_pos = TrimerPos(cur_row_start.x + offset / 2, cur_row_start.y, offset % 2);
					break;
				case 1:
					cur_pos =
						TrimerPos(cur_row_start.x - offset / 2 - offset % 2, cur_row_start.y + offset / 2, offset % 2);
					break;
				case 2:
					cur_pos = TrimerPos(cur_row_start.x, cur_row_start.y - (offset + 1) / 2, offset % 2);
					break;
				}

				offset += 3;

				if (n++ >= vacancies)
				{
					trimer_occupations[cur_pos.canonical(w, h).index(w)] = true;
					for (auto& [pos, rel] : cur_pos.get_clusters_wrel(w, h))
						vertex_occupations[pos.index(w)].occupations |= rel.occupations;
				}
			}

			switch (orientation)
			{
			case 0:
				cur_row_start = TrimerPos(cur_row_start.x, cur_row_start.y + 2, 0);
				break;
			case 1:
				cur_row_start = TrimerPos(cur_row_start.x - 2, cur_row_start.y, 0);
				break;
			case 2:
				cur_row_start = TrimerPos(cur_row_start.x + 2, cur_row_start.y - 2, 0);
				break;
			}
		}

		j4_total = -1;
		clusters_total = 0;
	}

	template <typename Rng> void load_from(std::string file, Rng& rng, int vacancies)
	{
		std::fill(trimer_occupations.begin(), trimer_occupations.end(), false);
		std::fill(vertex_occupations.begin(), vertex_occupations.end(), Cluster());

		std::ifstream ifs(file);
		if (ifs.fail())
			throw "random file not found";

		std::string line;
		std::vector<std::string> lines;
		std::vector<TrimerPos> positions;
		while (std::getline(ifs, line))
			lines.push_back(line);

		int randline = (int)(rng() % (2 * lines.size() / 3)) + (int)lines.size() / 3;
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

			positions.push_back(TrimerPos(nums[0], nums[1], nums[2]));
		}

		if (positions.size() != (uint)(w * h / 3))
			throw "file loading failed";

		std::shuffle(positions.begin(), positions.end(), rng);
		for (uint i = vacancies; i < positions.size(); i++)
		{
			const auto& pos = positions[i];
			trimer_occupations[pos.index(w)] = true;
			for (auto& [npos, rel] : pos.get_clusters_wrel(w, h))
				vertex_occupations[npos.index(w)].occupations |= rel.occupations;
		}

		j4_total = -1;
		clusters_total = -1;
	}

	template <typename Rng> void reconfigure_root3(Rng& rng, int vacancies = 0)
	{
		if (w != h || w % 6)
			throw "";

		std::fill(trimer_occupations.begin(), trimer_occupations.end(), false);
		std::fill(vertex_occupations.begin(), vertex_occupations.end(), Cluster());

		int offset = (int)(rng() % 6);

		int n = 0;
		for (int i = 0; i < w; i += 3)
			for (int j = 0; j < w; j += 3)
				for (int s = 0; s < 3; s++)
					if (n++ >= vacancies)
					{
						TrimerPos pos;
						pos = TrimerPos(i + offset / 2 + s, j + s, offset % 2);
						trimer_occupations[pos.canonical(w, h).index(w)] = true;
						for (auto& [pos, rel] : pos.get_clusters_wrel(w, h))
							vertex_occupations[pos.index(w)].occupations |= rel.occupations;
					}

		j4_total = 0;
		clusters_total = 0;
	}
};
const std::array<std::array<MonomerPos, 6>, 2> Sample::j4_neighbor_list = {
	std::array<MonomerPos, 6>{MonomerPos{0, 1}, {1, 0}, {0, -2}, {1, -2}, {-2, 0}, {-2, 1}},
	{MonomerPos{0, 2}, {-1, 2}, {2, 0}, {2, -1}, {0, -1}, {-1, 0}}};
const std::array<MonomerPos, 3> Sample::j6_neighbor_list = std::array<MonomerPos, 3>{MonomerPos{0, 2}, {2, 0}, {-2, 2}};

struct SampleDimer
{
  private:
	std::vector<TrimerPos> _pocket, _Abar;
	std::vector<std::pair<MonomerPos, Cluster>> _Abar_entries;
	std::unordered_map<TrimerPos, InteractionCount> _candidates;

  public:
	int32_t w, h;
	std::vector<bool> trimer_occupations;
	std::vector<Cluster> vertex_occupations;

	SampleDimer() : w(0), h(0) {}

	SampleDimer(int32_t w, int32_t h) : w(w), h(h)
	{
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		vertex_occupations = std::vector<Cluster>(w * h);
	}

	SampleDimer(int32_t w, int32_t h, const std::vector<TrimerPos>& dimers) : w(w), h(h)
	{
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		for (auto& i : dimers)
			trimer_occupations[i.index(w)] = true;

		regenerate_occupation();
	}

	std::array<std::pair<MonomerPos, Cluster>, 2> get_dimer_clusters_wrel(TrimerPos tri) const
	{
		std::array<std::pair<MonomerPos, Cluster>, 2> r;
		if (tri.s == 0)
		{
			r[0] = {{tri.x, tri.y}, Cluster::relative(0, 0, 0)};
			r[1] = {{tri.x + 1, tri.y}, Cluster::relative(-1, 0, 0)};
		}
		else
		{
			r[0] = {{tri.x, tri.y}, Cluster::relative(0, 0, 1)};
			r[1] = {{tri.x, tri.y + 1}, Cluster::relative(0, -1, 1)};
		}

		for (auto& i : r)
			i.first = i.first.canonical(w, h);

		return r;
	}

	void regenerate_occupation()
	{
		vertex_occupations = std::vector<Cluster>(w * h);

		for (uint i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
			{
				auto tri = TrimerPos::from_index(i, w);

				for (auto& [occ, rel] : get_dimer_clusters_wrel(tri))
					vertex_occupations[occ.index(w)].occupations |= rel.occupations;
			}
		}
	}

	int calculate_winding_number(TrimerPos start) const
	{
		TrimerPos cpos = start;
		int tot = 0;
		int sign = 1;

		while (true)
		{
			if (trimer_occupations[cpos.index(w)])
				tot += sign;

			switch (start.s)
			{
			case 0:
				cpos = TrimerPos(cpos.x, cpos.y + 1, 0);
				break;
			case 1:
				cpos = TrimerPos(cpos.x + 1, cpos.y, 1);
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
			ret.push_back(calculate_winding_number(TrimerPos(0, 0, dir)));

		return ret;
	}

	TrimerPos dimer_reflect(TrimerPos pos, MonomerPos center, int dir)
	{
		auto p = pos.center_at(center, w, h);
		int32_t cx = center.x, cy = center.y;
		TrimerPos np;
		switch (dir)
		{
		case 0:
			np = !p.s ? TrimerPos(cx + p.x, cy - p.y, 0) : TrimerPos(cx + p.x, cy - p.y - 1, 1);
			break;
		case 1:
			np = TrimerPos(cx + p.y, cy + p.x, 1 - p.s);
			break;
		case 2:
			np = !p.s ? TrimerPos(cx - p.x - 1, cy + p.y, 0) : TrimerPos(cx - p.x, cy + p.y, 1);
			break;
		case 3:
			np = !p.s ? TrimerPos(cx - p.y, cy - p.x - 1, 1) : TrimerPos(cx - p.y - 1, cy - p.x, 0);
			break;
		}

		return np.canonical(w, h);
	}

	template <typename Rng> void pocket_move(Rng& rng)
	{
		MonomerPos symc;
		int syma;

		TrimerPos seed(0, 0, 2);
		while (seed.s == 2)
		{
			uint candidate = (uint)(rng() % trimer_occupations.size());
			symc = MonomerPos((int)(rng() % w), (int)(rng() % h));
			syma = (int)(rng() % 4);

			if (trimer_occupations[candidate])
			{
				seed = TrimerPos::from_index(candidate, w);
				if (trimer_occupations[dimer_reflect(seed, symc, syma).index(w)])
					seed = TrimerPos(0, 0, 2);
			}
		}

		_Abar.clear();
		_Abar_entries.clear();
		_pocket.clear();
		_pocket.push_back(seed);

		trimer_occupations[seed.index(w)] = false;
		for (auto& [i, occ] : get_dimer_clusters_wrel(seed))
		{
			ASSERT((vertex_occupations[i.index(w)].occupations & occ.occupations) != 0)
			vertex_occupations[i.index(w)].occupations &= (uint8_t)~occ.occupations;
		}

		while (_pocket.size() > 0)
		{
			auto el = _pocket.back();
			_pocket.pop_back();

			auto moved = dimer_reflect(el, symc, syma);
			_Abar.push_back(moved);

			_candidates.clear();

			// add target overlaps to candidates
			for (const auto& [i, rel] : get_dimer_clusters_wrel(moved))
			{
				_Abar_entries.push_back(std::make_pair(i, rel));
				auto target_occ = vertex_occupations[i.index(w)].occupations;

				if (target_occ > 0)
				{
					for (int dx = -1; dx <= 0; dx++)
						for (int dy = -1; dy <= 0; dy++)
							for (int s = 0; s < 2; s++)
								if ((target_occ & Cluster::relative(dx, dy, s).occupations) != 0)
								{
									auto overlap = TrimerPos(i.x + dx, i.y + dy, s).canonical(w, h);
									_candidates[overlap].u++;
								}
				}
			}

			for (auto& [pos, interactions] : _candidates)
			{
				if (trimer_occupations[dimer_reflect(pos, symc, syma).index(w)] == false)
				{
					_pocket.push_back(pos);
					trimer_occupations[pos.index(w)] = false;
					for (auto& [cluster, rel] : get_dimer_clusters_wrel(pos))
					{
						ASSERT((vertex_occupations[cluster.index(w)].occupations & rel.occupations) != 0)
						vertex_occupations[cluster.index(w)].occupations &= (uint8_t)~rel.occupations;
					}
				}
			}
		}

		for (auto& i : _Abar)
		{
			ASSERT(trimer_occupations[i.index(w)] == false)
			trimer_occupations[i.index(w)] = true;
		}

		for (auto& [vertex, occ] : _Abar_entries)
		{
			ASSERT((vertex_occupations[vertex.index(w)].occupations & occ.occupations) == 0)
			vertex_occupations[vertex.index(w)].occupations |= occ.occupations;
		}

#ifdef TEST
		auto occupations = vertex_occupations;
		regenerate_occupation();
		ASSERT(occupations == vertex_occupations)
#endif
	}
};

struct FullEnumerator
{
	using Mask = std::vector<bool>;
	using vii = std::vector<std::pair<int, int>>;
	struct MaskShape
	{
		int size;
		vii slots;
	};

	template<typename T>
	static T rotate(const T& x, int y)
	{
		T ret;
		for (int i = 0; i < (intx.size(); i++)
			ret.push_back(x[(i + y) % x.size()]);
		return ret;
	}

	static MaskShape shape_of(const Mask& mask)
	{
		if (mask.size() == 0)
			return MaskShape{0, vii()};

		if (std::all_of(mask.begin(), mask.end(), [](bool x) { return !x; }))
		{
			// periodic
			return MaskShape{(int)mask.size(), vii{std::pair<int, int>{0, (int)mask.size()}}};
		}

		int shift = (int)(std::find(mask.begin(), mask.end(), true) - mask.begin());
		vii ret;
		int begin = -1;

		for (int i = 0; i < (int)mask.size(); i++)
		{
			if (mask[(i + shift) % mask.size()] == 1)
			{
				if (begin < i - 1)
					ret.push_back({(begin + shift + 1) % mask.size(), i - begin - 1});
				begin = i;
			}
		}
		if (begin < (int)mask.size() - 1)
			ret.push_back({(begin + shift + 1) % mask.size(), mask.size() - begin - 1});

		return MaskShape{(int)mask.size(), ret};
	}

	static Mask mask_above(const std::vector<int>& row)
	{
		if (row.size() == 0)
			return Mask();

		Mask places;
		for (int i = 0; i < (int)row.size(); i++)
		{
			if (row[i] == 1 || row[i] == -1 || row[(i+1)%row.size()] == -1)
				places.push_back(true);
			else
				places.push_back(false);
		}
		return places;
	}

	static bool fill_reset(const MaskShape& shape, int shift, bool periodic, int begin, std::vector<int>& row)
	{
		for (auto& [start, l] : shape.slots)
		{
			int shifted_start = (start - shift) % (int)row.size();
			if (shifted_start + l >= begin)
			{
				int true_start = std::max(shifted_start, begin);
				int true_length = shifted_start + l - true_start;
				if (periodic && true_length % 2 == 1 && row[0] == -1)
				{
					if (true_length < 3)
						return false;
					
					for (int i = true_start; i < shifted_start + l - 3; i += 2)
					{
						row[i] = 1;
						row[i + 1] = 2;
					}
					row[shifted_start + l - 3] = -1;
					row[shifted_start + l - 2] = 1;
					row[shifted_start + l - 1] = 2;
				}
				else
				{
					for (int i = true_start; i < shifted_start + l - 1; i += 2)
					{
						row[i] = 1;
						row[i + 1] = 2;
					}

					if (true_length % 2 == 1)
						row[shifted_start + l - 1] = -1;
				}
			}
		}
		return true;
	}

	static std::vector<int> advance(const MaskShape& shape, std::vector<int> prev)
	{
		if (shape.slots.size() == 0)
		{
			if (prev.size() == 0)
				return std::vector<int>(shape.size);
			else
				return std::vector<int>();
		}

		bool periodic;
		int shift;
		if (shape.slots[0].second == shape.size)
		{
			periodic = true;
			shift = 0;
			if (prev.size() > 0)
			{
				if (prev[0] == 1)
					return rotate(prev, 1);
				else if (prev[0] == 2)
					prev = rotate(prev, (int)prev.size() - 1);
			}
		}
		else
		{
			periodic = false;
			shift = shape.slots[0].first;
		}

		if (prev.size() == 0)
		{
			std::vector<int> ret(shape.size);
			fill_reset(shape, shift, periodic, 0, ret);
			return rotate(ret, (int)ret.size() - shift);
		}

		prev = rotate(prev, shift);
		auto ret = prev;

		for (int i = (int)prev.size() - 1; i >= 0; i--)
		{
			if (i >= 3 && (ret[(i+1)%ret.size()] == 0 || ret[(i+1)%ret.size()] == 1) &&
					ret[i] == 2 && ret[i-2] == 2 && (i < 4 || ret[i-4] == 2 || ret[i-4] == 0))
			{
				auto temp_ret = ret;
				temp_ret[i-3] = -1;
				temp_ret[i-2] = 1;
				temp_ret[i-1] = 2;
				temp_ret[i] = -1;
				if (!fill_reset(shape, shift, periodic, i+1, temp_ret))
					continue;
				ret = temp_ret;
				goto done;
			}
			if (i >= 2 && ret[i] == -1 && ret[i-1] == 2 &&
				(i < 3 || ret[i-3] == 2 || ret[i-3] == 0))
			{
				auto temp_ret = ret;
				temp_ret[i-2] = -1;
				temp_ret[i-1] = 1;
				temp_ret[i] = 2;
				if (!fill_reset(shape, shift, periodic, i+1, temp_ret))
					continue;
				ret = temp_ret;
				goto done;
			}
		}
		return std::vector<int>();

	done:
		return rotate(ret, ret.size() - shift);
	}

	std::vector<bool> init_mask;
	int height;
	std::vector<std::pair<MaskShape, std::vector<int>> dfs;
	FullEnumerator(int w, int h) : init_mask(w), height(h) { }

	bool next_mask()
	{
		int ind = init_mask.size() - 1;
		while (ind >= 0 && init_mask[ind])
		{
			init_mask[ind] = !init_mask[ind];
			ind--;
		}
		if (ind < 0)
			return false;

		init_mask[ind] = !init_mask[ind];
		return true;
	}

	bool next(Sample& s)
	{
		while (true)
		{
			if (dfs.size() == 0)
			{
				for (auto i : init_mask)
					std::cout << i << " ";
				std::cout << std::endl;

				auto init_shape = shape_of(init_mask);
				dfs.push_back({ init_shape, advance(init_shape, std::vector<int>()) });
			}

			while (true)
			{
				if (dfs.back().second.size() == 0)
				{
					dfs.pop_back();
					if (dfs.size() == 0)
						if (!next_mask())
							return false;
					dfs[-1].second = advance(dfs.back().first, dfs.back().second);
					continue;
				}

				if (dfs.size() < height)
				{
					auto prev_shape = shape_of(mask_above(dfs.back().second));
					auto new_row = advance(prev_shape, std::vector<int>());
					dfs.push_back({ prev_shape, new_row });
					continue;
				}

				if (mask_above(dfs.back().second == init_mask))
				{
					Sample sample;

					dfs.back().second = advance(dfs.back().first, dfs.back().second);
					return sample;
				}

				dfs.back().second = advance(dfs.back().first, dfs.back().second);
			}
		}
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
	std::ofstream ofwinding;
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

	PTWorker() : options(Options()) {}

	PTWorker(const Options& opt, double temperature, bool output) : options(opt), temperature(temperature), timing(2, 1)
	{
		sample = Sample(opt.domain_length, opt.domain_length);

		rng.seed(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		sample.initialize(opt, rng);

		if (output)
			enable_io();
	}

	void enable_io()
	{
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
			ofwinding = std::ofstream(basepath / "winding-histogram.dat", std::ios::binary);
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
				Sample copy = sample;
				copy.metropolis_move(rng);
				copy.calculate_energy();

				double d_energy =
					(copy.j4_total - sample.j4_total) * j4 + (copy.clusters_total - sample.clusters_total) * u;
				if (uniform(rng) <= std::exp(-d_energy))
					std::swap(copy, sample);
			}
			else
				sample.pocket_move(rng, u, j4, Sample::boltzmann_cascader());
		}

		total_steps += options.decorr_interval;

		auto end1 = std::chrono::high_resolution_clock::now();
		sample.calculate_energy();

		std::vector<double> timings(2);
		timings[0] = (double)std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();

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
				for (uint j = 0; j < sample.trimer_occupations.size(); j++)
					if (sample.trimer_occupations[j])
						ofconf << TrimerPos::from_index(j, sample.w) << " ";
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
				write_binary<float>(ofwinding, (float)energy);
				auto windings = sample.calculate_winding_numbers();
				for (int i = 0; i < 9; i++)
					write_binary<int16_t>(ofwinding, (int16_t)windings[i]);
				ofwinding.flush();
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

			double u_factor = (chains[i].sample.clusters_total - chains[i + 1].sample.clusters_total) * options.u;
			double j4_factor = (chains[i].sample.j4_total - chains[i + 1].sample.j4_total) * options.j4;

			double action;
			if (u_factor + j4_factor == 0)
				action = 0;
			else
				action = (1 / chains[i + 1].temperature - 1 / chains[i].temperature) * (u_factor + j4_factor);

			double accept = std::exp(-action);
			acceptances[i] += std::min(1., accept);

			if (uniform(rng) < accept)
				std::swap(chains[i].sample, chains[i + 1].sample);
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
			Sample copy = sample;
			copy.pocket_move(rng, infinity, 0, Sample::boltzmann_cascader());
			copy.calculate_energy();

			if (copy.j4_total > max_energy)
			{
				if (copy.j4_total < sample.j4_total)
					std::swap(copy, sample);
				continue;
			}
			if (copy.j4_total < min_energy)
			{
				if (copy.j4_total > sample.j4_total)
					std::swap(copy, sample);
				continue;
			}

			if (uniform(rng) <= std::exp(get_log_dos(sample.j4_total) - get_log_dos(copy.j4_total)))
				std::swap(copy, sample);
		}

		log_dos[sample.j4_total] += factor;
		hist[sample.j4_total] += 1;
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

			if (chains[i].sample.j4_total < chains[i + 1].min_energy)
				continue;
			if (chains[i + 1].sample.j4_total > chains[i].max_energy)
				continue;

			double accept = std::exp(chains[i].get_log_dos(chains[i].sample.j4_total) +
									 chains[i + 1].get_log_dos(chains[i + 1].sample.j4_total) -
									 chains[i].get_log_dos(chains[i + 1].sample.j4_total) -
									 chains[i + 1].get_log_dos(chains[i].sample.j4_total));

			if (uniform(rng) < accept)
				std::swap(chains[i].sample, chains[i + 1].sample);
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
	options.out_winding_histogram = options.out_histogram;

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

		std::vector<TrimerPos> positions;
		for (int i = 0; i < options.domain_length; i += 2)
			for (int j = 0; j < options.domain_length; j += 1)
				positions.push_back(TrimerPos(i, j, 0));

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
				for (uint j = 0; j < sample.trimer_occupations.size(); j++)
					if (sample.trimer_occupations[j])
						ofconf << TrimerPos::from_index(j, sample.w) << " ";
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

void test_pocket()
{
	Sample sample(6, 6);
	std::minstd_rand rng;
	sample.reconfigure_brick_wall(rng);
	for (int i = 0; i < 100; i++)
		sample.pocket_move(rng, 1, 0.01, Sample::boltzmann_cascader());

	std::cout << "pocket test passed" << std::endl;
}

void test_brickwall()
{
	Sample sample(48, 48);
	std::minstd_rand rng;
	for (int i = 0; i < 20; i++)
	{
		sample.reconfigure_brick_wall(rng);
		sample.calculate_energy();
		TEST_ASSERT2(sample.j4_total == 48 * 48 / 2, sample.j4_total)
	}
}

#ifndef TEST
int main(int argc, char** argv) { sim(argc, argv); }
#else
int main()
{
	test_optimizer();
	test_pocket();
	test_brickwall();
}
#endif
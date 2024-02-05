#include <atomic>
#include <chrono>
#include <complex>
#include <filesystem>
#include <fstream>
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

#else
#define ASSERT(x)
#define TEST_ASSERT(x)
#endif

int32_t pmod(int32_t a, int32_t b) { return (a % b + b) % b; }

const double infinity = std::numeric_limits<double>::infinity();
using complex = std::complex<double>;

struct MonomerPos
{
	int32_t x, y;

	MonomerPos() : x(0), y(0) {}
	MonomerPos(int32_t x, int32_t y) : x(x), y(y) {}

	MonomerPos rotate(int8_t dir)
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
	MonomerPos center_at(MonomerPos pos, int32_t w, int32_t h)
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

	bool operator==(const Cluster& other) const { return occupations == other.occupations; }
};

struct TrimerPos
{
	int32_t x, y;
	int8_t s;

	TrimerPos() : x(0), y(0), s(0) {}
	TrimerPos(int32_t x, int32_t y, int8_t s) : x(x), y(y), s(s) {}

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

	TrimerPos reflect(MonomerPos center, int8_t dir, int32_t w, int32_t h) const
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
	Accumulator(int n, int interval) : mean(n), m2(n), interval(interval) {}

	void record(const std::vector<T>& vals)
	{
		total++;

		double inv_total = (1. / total);

		for (size_t i = 0; i < vals.size(); i++)
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

struct Sample
{
	struct PairInteraction
	{
		int j4 = 0;
		int u = 0;
	};

  private:
	std::vector<TrimerPos> _pocket, _Abar;
	std::vector<std::pair<MonomerPos, Cluster>> _Abar_entries;
	std::unordered_map<TrimerPos, PairInteraction> _candidates;
	std::unordered_map<TrimerPos, std::vector<complex>> _trimer_contributions;

  public:
	static const std::array<std::array<MonomerPos, 6>, 2> j4_neighbor_list;

	int32_t w, h;
	int j4_total, clusters_total;
	std::vector<bool> trimer_occupations;
	std::vector<Cluster> vertex_occupations;

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

	void regenerate_occupation()
	{
		vertex_occupations = std::vector<Cluster>(w * h);

		for (size_t i = 0; i < trimer_occupations.size(); i++)
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

		for (size_t i = 0; i < trimer_occupations.size(); i++)
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

	std::vector<MonomerPos> all_monomers() const
	{
		std::vector<MonomerPos> r;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
			{
				if (vertex_occupations[MonomerPos(i, j).index(w)].occupations == 0)
					r.emplace_back(i, j);
			}

		return r;
	}

	void record_dimer_monomer_correlations(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h);
		std::vector<MonomerPos> monos = all_monomers();

		std::vector<std::pair<MonomerPos, int>> dimers;
		std::vector<MonomerPos> disjoint_monos;
		std::unordered_set<MonomerPos> connected_monos;

		for (size_t i = 0; i < monos.size(); i++)
		{
			bool found = false;
			for (size_t j = i + 1; j < monos.size(); j++)
			{
				MonomerPos origin;
				int rotation = -1;

				const std::vector<int> dx = {1, 0, -1};
				const std::vector<int> dy = {0, 1, 1};

				for (int r = 0; r < 3; r++)
				{
					if (monos[i].x + dx[r] == monos[j].x && monos[i].y + dy[r] == monos[j].y)
					{
						origin = monos[i];
						rotation = r;
						break;
					}
					if (monos[j].x + dx[r] == monos[i].x && monos[j].y + dy[r] == monos[i].y)
					{
						origin = monos[j];
						rotation = r;
						break;
					}
				}

				if (rotation >= 0)
				{
					dimers.emplace_back(origin, rotation);
					found = true;
					connected_monos.insert(monos[i]);
					connected_monos.insert(monos[j]);
				}
			}

			if (!found && !connected_monos.contains(monos[i]))
				disjoint_monos.push_back(monos[i]);
		}

		for (auto& i : dimers)
			for (auto& j : disjoint_monos)
				vals[j.center_at(i.first, w, h).rotate(-i.second).canonical(w, h).index(w)]++;

		a.record(vals);
	}

	void record_trimer_correlations(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h * 2);
		for (size_t i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
				for (size_t j = i + 1; j < trimer_occupations.size(); j++)
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

	void record_order_parameters(Accumulator<double>& a) { a.record(std::vector<double>()); }

	template <typename Rng>
	void record_partial_trimer_correlations(Accumulator<double>& a, Rng& rng, double fraction) const
	{
		std::vector<double> vals(w * h * 2);
		std::uniform_real_distribution<> uniform(0., 1.);

		int seen = 0;
		for (size_t i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i] && uniform(rng) <= fraction)
			{
				seen++;
				for (size_t j = 0; j < trimer_occupations.size(); j++)
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
	void record_partial_monomer_correlations(Accumulator<double>& a, Rng& rng, double fraction) const
	{
		std::vector<double> vals(w * h);
		std::vector<MonomerPos> pos;

		for (uint i = 0; i < vertex_occupations.size(); i++)
			if (vertex_occupations[i].occupations == 0)
				pos.push_back(MonomerPos::from_index(i, w));

		std::shuffle(pos.begin(), pos.end(), rng);
		size_t sample_count = std::min(pos.size(), (size_t)(fraction * pos.size()) + 1);

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

		double inv_count = 1. / pos.size();
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

	void record_energy(Accumulator<double>& a, double u, double j4)
	{
		std::vector<double> vals(3);
		calculate_energy();

		vals[0] = clusters_total;
		vals[1] = j4_total;
		vals[2] = clusters_total * u + j4_total * j4;

		a.record(vals);
	}

	template <typename Rng> void pocket_move(Rng& rng, double u, double j4)
	{
		MonomerPos symc;
		int syma;

		TrimerPos seed(0, 0, 2);
		while (seed.s == 2)
		{
			uint32_t candidate = rng() % trimer_occupations.size();
			symc = MonomerPos(rng() % w, rng() % h);
			syma = rng() % 6;

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
			vertex_occupations[i.index(w)].occupations &= ~occ.occupations;
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

				if (target_occ > 0 && u != 0 && u > -infinity)
					for (int dx = -1; dx <= 0; dx++)
						for (int dy = -1; dy <= 0; dy++)
							for (int s = 0; s < 2; s++)
								if ((target_occ & Cluster::relative(dx, dy, s).occupations) != 0)
								{
									auto overlap = TrimerPos(i.x + dx, i.y + dy, s).canonical(w, h);
									_candidates[overlap].u++;
								}
			}

			// add source overlaps to candidates
			if (u != 0 && u < infinity)
				for (auto& [i, rel] : el.get_clusters_wrel(w, h))
				{
					auto orig_occ = vertex_occupations[i.index(w)].occupations;

					if (orig_occ > 0)
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
				double u_factor = interactions.u != 0 ? u * interactions.u : 0;
				double j4_factor = interactions.j4 != 0 ? j4 * interactions.j4 : 0;

				if (u_factor + j4_factor == infinity || uniform(rng) < 1 - std::exp(-(u_factor + j4_factor)))
					if (trimer_occupations[pos.reflect(symc, syma, w, h).index(w)] == false)
					{
						_pocket.push_back(pos);
						trimer_occupations[pos.index(w)] = false;
						for (auto& [cluster, rel] : pos.get_clusters_wrel(w, h))
						{
							ASSERT((vertex_occupations[cluster.index(w)].occupations & rel.occupations) != 0)
							vertex_occupations[cluster.index(w)].occupations &= ~rel.occupations;
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
			uint32_t candidate = rng() % trimer_occupations.size();
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
			uint32_t candidate = rng() % trimer_occupations.size();
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
			popc &= ~rel.occupations;
		}
		for (const auto& [occ, rel] : dest.get_clusters_wrel(w, h))
		{
			auto& popc = vertex_occupations[occ.index(w)].occupations;
			ASSERT((popc & rel.occupations) == 0);

			// difference due to adding one one
			// ((x+1)^2 - (x+1)) - (x^2 - x) = x
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

		int orientation = rng() % 3;
		int phase = rng() % 3;

		std::vector<int> phase_conf;
		phase_conf.push_back(phase);

		std::uniform_real_distribution<> uniform(0., 1.);

		for (int i = 1; i < h / 2; i++)
		{
			if (phase_conf[i - 1] == phase_conf[0])
				phase_conf.push_back((rng() % 2) ? (phase_conf[0] + 1) % 3 : (phase_conf[0] + 2) % 3);
			else
			{
				int n = h / 2 - i;
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
			uint offset = phase_conf[row] + row + 3 * (rng() % w);

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
	}

	template <typename Rng> void reconfigure_root3(Rng& rng, int vacancies = 0)
	{
		if (w != h || w % 6)
			throw "";

		std::fill(trimer_occupations.begin(), trimer_occupations.end(), false);
		std::fill(vertex_occupations.begin(), vertex_occupations.end(), Cluster());

		int offset = rng() % 6;

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
	}
};
const std::array<std::array<MonomerPos, 6>, 2> Sample::j4_neighbor_list = {
	std::array<MonomerPos, 6>{MonomerPos{0, 1}, {1, 0}, {0, -2}, {1, -2}, {-2, 0}, {-2, 1}},
	{MonomerPos{0, 2}, {-1, 2}, {2, 0}, {2, -1}, {0, -1}, {-1, 0}}};

struct Options
{
	int length;
	int mono_vacancies;

	bool out_monomono;
	bool out_clustercount;
	bool out_monodi;
	bool out_tritri;
	bool out_energy;
	bool out_positions;
	bool out_order;

	bool idealbrickwall;
	bool idealrt3;
	bool metropolis;

	int total_steps;
	int decorr_interval;
	int accumulator_interval;
	int swap_interval = 10;

	double j4_over_u;
	std::vector<double> temperatures;

	std::string directory;
};

struct PTWorker
{
	const Options& options;

	double temperature;
	Sample sample;
	std::minstd_rand rng;

	std::ofstream ofmonomono;
	std::ofstream ofclustercount;
	std::ofstream ofmonodi;
	std::ofstream oftritri;
	std::ofstream ofenergy;
	std::ofstream ofconf;
	std::ofstream oforder;
	std::ofstream ofinfo;

	Accumulator<double> amonomono;
	Accumulator<double> aorder;
	Accumulator<double> aclustercount;
	Accumulator<double> amonodi;
	Accumulator<double> atritri;
	Accumulator<double> aenergy;

	Accumulator<double> timing;

	int total_steps = 0;

	PTWorker(const Options& opt, double temperature)
		: options(opt), temperature(temperature), sample(opt.length, opt.length), timing(3, 1)
	{
		rng.seed(
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		if (opt.j4_over_u < 0)
			sample.reconfigure_brick_wall(rng, opt.mono_vacancies / 3);
		else
			sample.reconfigure_root3(rng, opt.mono_vacancies / 3);

		std::filesystem::path basepath;
		int runid = 0;
		do
		{
			std::stringstream ss;
			ss << options.length << "x" << options.length << "_r-" << options.mono_vacancies << "_t" << std::fixed
			   << std::setprecision(6) << temperature << "_j" << std::setprecision(3) << options.j4_over_u << "_"
			   << options.total_steps << "." << options.decorr_interval << "_" << runid++;
			basepath = std::filesystem::path("data") / options.directory / ss.str();
		} while (std::filesystem::exists(basepath));
		std::filesystem::create_directories(basepath);
		std::cout << "saving to " << basepath << std::endl;

		ofmonomono = std::ofstream(basepath / "mono-mono.dat");
		ofclustercount = std::ofstream(basepath / "cluster-count.dat");
		ofmonodi = std::ofstream(basepath / "mono-di.dat");
		oftritri = std::ofstream(basepath / "tri-tri.dat");
		ofenergy = std::ofstream(basepath / "energy.dat");
		ofconf = std::ofstream(basepath / "positions.dat");
		oforder = std::ofstream(basepath / "order.dat");
		ofinfo = std::ofstream(basepath / "info.dat");

		ofinfo
			<< "The order parameters are: |n(M)|, |n(M)|^2, |n(M)|^4, |n(K)|, |n(K)|^2, |n(K)|^4, A-B, |A-B|^2, |A-B|^4"
			<< std::endl;

		amonomono = Accumulator<double>(options.length * options.length, options.accumulator_interval);
		aorder = Accumulator<double>(9, options.accumulator_interval);
		aclustercount = Accumulator<double>(7, options.accumulator_interval);
		amonodi = Accumulator<double>(options.length * options.length, options.accumulator_interval);
		atritri = Accumulator<double>(options.length * options.length * 2, options.accumulator_interval);
		aenergy = Accumulator<double>(3, options.accumulator_interval);
	}

	void step()
	{
		auto begin = std::chrono::high_resolution_clock::now();

		double j4 = temperature > 0 ? options.j4_over_u / temperature : options.j4_over_u;
		double u = 1 / temperature;

		std::uniform_real_distribution<> uniform(0., 1.);
		for (int i = 0; i < options.decorr_interval; i++)
		{
			if (options.idealbrickwall)
				sample.reconfigure_brick_wall(rng, false);
			else if (options.idealrt3)
				sample.reconfigure_root3(rng);
			else if (options.metropolis)
			{
				Sample copy = sample;
				copy.metropolis_move(rng);

				double d_energy =
					(copy.j4_total - sample.j4_total) * j4 + (copy.clusters_total - sample.clusters_total) * u;
				if (uniform(rng) <= std::exp(-d_energy))
					std::swap(copy, sample);
			}
			else
				sample.pocket_move(rng, u, j4);
		}

		auto end1 = std::chrono::high_resolution_clock::now();

		if (options.out_monomono)
			sample.record_partial_monomer_correlations(amonomono, rng, 32. / (options.mono_vacancies));
		if (options.out_clustercount)
			sample.record_cluster_count(aclustercount);
		if (options.out_monodi)
			sample.record_dimer_monomer_correlations(amonodi);
		if (options.out_tritri)
			sample.record_partial_trimer_correlations(atritri, rng, 64. / (sample.w * sample.h / 3));
		if (options.out_energy)
			sample.record_energy(aenergy, 1, options.j4_over_u);
		if (options.out_order)
			sample.record_order_parameters(aorder);

		auto end2 = std::chrono::high_resolution_clock::now();

		amonomono.write(ofmonomono);
		aclustercount.write(ofclustercount);
		amonodi.write(ofmonodi);
		atritri.write(oftritri);
		aenergy.write(ofenergy);
		aorder.write(oforder);

		if (options.out_positions)
		{
			for (size_t j = 0; j < sample.trimer_occupations.size(); j++)
				if (sample.trimer_occupations[j])
					ofconf << TrimerPos::from_index(j, sample.w) << " ";
			ofconf << std::endl;
		}

		auto end3 = std::chrono::high_resolution_clock::now();

		std::vector<double> timings(3);
		timings[0] = std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
		timings[1] = std::chrono::duration_cast<std::chrono::microseconds>(end2 - end1).count();
		timings[2] = std::chrono::duration_cast<std::chrono::microseconds>(end3 - end2).count();

		timing.record(timings);
	}

	void final_output()
	{
		amonomono.write(ofmonomono, true);
		aclustercount.write(ofclustercount, true);
		amonodi.write(ofmonodi, true);
		atritri.write(oftritri, true);
		aenergy.write(ofenergy, true);
		aorder.write(oforder, true);
	}
};

struct PTEnsemble
{
	const Options& options;

	std::minstd_rand rng;
	std::vector<PTWorker> chains;
	int parity = 0;

	Accumulator<double> timing;

	PTEnsemble(const Options& options) : options(options), timing(2, 1)
	{
		rng.seed(
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				.count());

		for (auto& temp : options.temperatures)
			chains.push_back(PTWorker(options, temp));
	}

	int steps_done = 0;
	int last_output = 0;

	void step_all()
	{
		std::uniform_real_distribution<> uniform(0., 1.);
		std::vector<std::thread> threads;

		if (chains.size() > 1)
		{
			std::vector<double> timings(2);

			auto begin = std::chrono::high_resolution_clock::now();

			for (size_t i = 0; i < chains.size(); i++)
				threads.push_back(std::thread([this, i] {
					for (int j = 0; j < options.swap_interval; j++)
						chains[i].step();
				}));

			auto end1 = std::chrono::high_resolution_clock::now();

			for (auto& thread : threads)
				thread.join();

			auto end2 = std::chrono::high_resolution_clock::now();

			timings[0] = std::chrono::duration_cast<std::chrono::microseconds>(end1 - begin).count();
			timings[1] = std::chrono::duration_cast<std::chrono::microseconds>(end2 - end1).count();
			timing.record(timings);
		}
		else
		{
			for (int i = 0; i < options.swap_interval; i++)
				chains[0].step();
		}

		for (size_t i = parity++ % 2; i < chains.size() - 1; i += 2)
		{
			chains[i].sample.calculate_energy();
			chains[i + 1].sample.calculate_energy();

			int u_factor = chains[i].sample.clusters_total - chains[i + 1].sample.clusters_total;
			int j4_factor = (chains[i].sample.j4_total - chains[i + 1].sample.j4_total) * options.j4_over_u;

			double action = (1 / chains[i + 1].temperature - 1 / chains[i].temperature) * (u_factor + j4_factor);

			if (uniform(rng) < std::exp(-action))
				std::swap(chains[i].sample, chains[i + 1].sample);
		}

		steps_done += options.decorr_interval;

		if (steps_done >= last_output + 2500)
		{
			last_output = steps_done;

			std::cout << "Step " << steps_done << " of " << options.total_steps << std::endl;
			std::cout << "Master " << timing.mean[0] << " " << timing.mean[1] << std::endl;
			timing.reset();

			for (auto& chain : chains)
			{
				std::cout << "Thread" << chain.temperature << " " << chain.timing.mean[0] << " " << chain.timing.mean[1]
						  << " " << chain.timing.mean[2] << std::endl;
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

void sim(int argc, char** argv)
{
	if (argc < 10)
		exit(-1);

	Options options;
	options.length = std::stoi(std::string(argv[1]));
	options.mono_vacancies = std::stoi(std::string(argv[2])) * 3;
	options.j4_over_u = std::stod(std::string(argv[3]));

	std::string temperatures = std::string(argv[4]);
	std::istringstream ss(temperatures);
	std::string token;
	while (std::getline(ss, token, ','))
		options.temperatures.push_back(std::stod(token));

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
	options.out_positions = optstring.find('p') != std::string::npos;
	options.out_order = optstring.find('o') != std::string::npos;
	options.idealbrickwall = optstring.find('B') != std::string::npos;
	options.idealrt3 = optstring.find('3') != std::string::npos;
	options.metropolis = optstring.find('M') != std::string::npos;

	PTEnsemble ensemble(options);
	while (ensemble.steps_done < options.total_steps)
		ensemble.step_all();
	ensemble.final_output();
}

int main(int argc, char** argv) { sim(argc, argv); }
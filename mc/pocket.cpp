#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdlib.h>
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

	Accumulator(int n, int interval) : mean(n), m2(n), interval(interval) {}

	void record(const std::vector<T>& vals)
	{
		total++;

		for (size_t i = 0; i < vals.size(); i++)
		{
			double delta = vals[i] - mean[i];
			mean[i] += delta / total;
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

			total = 0;
			std::fill(mean.begin(), mean.end(), 0);
			std::fill(m2.begin(), m2.end(), 0);
		}
	}
};

struct Sample
{
	struct PairInteraction
	{
		int j4 = 0;
		int u = 0;
	};

	static const std::array<std::array<MonomerPos, 6>, 2> j4_neighbor_list;

	int32_t w, h;

	int j4_total, clusters_total;

	std::vector<bool> trimer_occupations;
	std::vector<Cluster> vertex_occupations;

	std::vector<TrimerPos> _pocket, _Abar;
	std::vector<std::pair<MonomerPos, Cluster>> _Abar_entries;
	std::unordered_map<TrimerPos, PairInteraction> _candidates;

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

	template <typename Rng>
	void record_partial_trimer_correlations(Accumulator<double>& a, Rng& rng, double probability) const
	{
		std::vector<double> vals(w * h * 2);
		std::uniform_real_distribution<> uniform(0., 1.);

		for (size_t i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i] && uniform(rng) <= probability)
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

		a.record(vals);
	}

	void record_monomers(Accumulator<double>& a) const
	{
		std::vector<double> vals(w * h);

		for (uint i = 0; i < vertex_occupations.size(); i++)
			if (vertex_occupations[i].occupations == 0)
				vals[i]++;

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
				vals[d.canonical(w, h).index(w)]++;
				vals[(-d).canonical(w, h).index(w)]++;
			}
		}

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

struct PTEnsemble
{
	struct Chain
	{
		double temp;
		Sample sample;
	};

	std::vector<Chain> chains;
	int parity;

	double j4;
	double u;

	template <typename Rng> void step_all(Rng& rng)
	{
		std::uniform_real_distribution<> uniform(0., 1.);

		for (size_t i = 0; i < chains.size(); i++)
		{
			chains[i].sample.pocket_move(rng, u / chains[i].temp, j4 / chains[i].temp);
		}

		for (size_t i = parity++ % 2; i < chains.size() - 1; i += 2)
		{
			double e0 = chains[i].sample.clusters_total * u + chains[i].sample.j4_total * j4;
			double e1 = chains[i + 1].sample.clusters_total * u + chains[i + 1].sample.j4_total * j4;

			double action = (1 / chains[i + 1].temp - 1 / chains[i].temp) * (e0 - e1);

			if (uniform(rng) < std::exp(-action))
				std::swap(chains[i].sample, chains[i + 1].sample);
		}
	}
};

void sim(int argc, char** argv)
{
	if (argc < 10)
		exit(-1);

	std::minstd_rand rng;
	rng.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				 .count());

	int s = std::stoi(std::string(argv[1]));

	auto sample = Sample(s, s);

	int vacancies = std::stoi(std::string(argv[2]));
	double j4_over_u = std::stod(std::string(argv[3]));
	double t_over_u = std::stod(std::string(argv[4]));

	if (j4_over_u < 0)
		sample.reconfigure_brick_wall(rng, vacancies);
	else
		sample.reconfigure_root3(rng, vacancies);

	int N = std::stoi(std::string(argv[5]));
	int stride = std::stoi(std::string(argv[6]));

	std::string options = std::string(argv[7]);
	int interval = std::stoi(std::string(argv[8]));

	std::string dir = std::string(argv[9]);

	std::filesystem::path basepath;
	int runid = 0;
	do
	{
		std::stringstream ss;
		ss << s << "x" << s << "_r-" << vacancies * 3 << "_t" << std::fixed << std::setprecision(6) << t_over_u << "_j"
		   << std::setprecision(3) << j4_over_u << "_" << N << "." << stride << "_" << runid++;
		basepath = std::filesystem::path("data") / dir / ss.str();
	} while (std::filesystem::exists(basepath));

	std::filesystem::create_directories(basepath);

	std::ofstream ofmonomono(basepath / "mono-mono.dat");
	std::ofstream ofclustercount(basepath / "cluster-count.dat");
	std::ofstream ofmonodi(basepath / "mono-di.dat");
	std::ofstream oftritri(basepath / "tri-tri.dat");
	std::ofstream ofenergy(basepath / "energy.dat");
	std::ofstream ofconf(basepath / "positions.dat");

	Accumulator<double> amonomono(s * s, interval);
	Accumulator<double> aclustercount(7, interval);
	Accumulator<double> amonodi(s * s, interval);
	Accumulator<double> atritri(s * s * 2, interval);
	Accumulator<double> aenergy(3, interval);

	bool monomono = options.find('m') != std::string::npos;
	bool clustercount = options.find('c') != std::string::npos;
	bool monodi = options.find('d') != std::string::npos;
	bool tritri = options.find('t') != std::string::npos;
	bool energy = options.find('e') != std::string::npos;
	bool conf = options.find('p') != std::string::npos;

	bool idealbrickwall = options.find('B') != std::string::npos;
	bool idealrt3 = options.find('3') != std::string::npos;
	bool metropolis = options.find('M') != std::string::npos;

	std::cout << "saving to " << basepath << std::endl;

	double j4 = j4_over_u / t_over_u;
	double u = 1 / t_over_u;
	std::uniform_real_distribution<> uniform(0., 1.);
	for (int i = 0; i < N; i++)
	{
		if (idealbrickwall)
			sample.reconfigure_brick_wall(rng, false);
		else if (idealrt3)
			sample.reconfigure_root3(rng);
		else if (metropolis)
		{
			Sample copy = sample;
			copy.metropolis_move(rng);

			double d_energy =
				(copy.j4_total - sample.j4_total) * j4 + (copy.clusters_total - sample.clusters_total) * u;
			if (uniform(rng) <= std::exp(-d_energy))
				// accept
				std::swap(copy, sample);
		}
		else
			sample.pocket_move(rng, u, j4);

		if (i % 1000 == 0)
		{
			std::cout << s << " " << i << std::endl;
		}

		if ((i + 1) % stride != 0)
			continue;

		if (monomono)
			sample.record_monomer_correlations(amonomono);
		if (clustercount)
			sample.record_cluster_count(aclustercount);
		if (monodi)
			sample.record_dimer_monomer_correlations(amonodi);
		if (tritri)
			sample.record_partial_trimer_correlations(atritri, rng, 3. / s);
		if (energy)
			sample.record_energy(aenergy, 1, j4_over_u);

		amonomono.write(ofmonomono);
		aclustercount.write(ofclustercount);
		amonodi.write(ofmonodi);
		atritri.write(oftritri);
		aenergy.write(ofenergy);

		if (conf)
		{
			for (size_t j = 0; j < sample.trimer_occupations.size(); j++)
				if (sample.trimer_occupations[j])
					ofconf << TrimerPos::from_index(j, sample.w) << " ";
			ofconf << std::endl;
		}
	}

	amonomono.write(ofmonomono, true);
	aclustercount.write(ofclustercount, true);
	amonodi.write(ofmonodi, true);
	atritri.write(oftritri, true);
	aenergy.write(ofenergy, true);
}

int main(int argc, char** argv) { sim(argc, argv); }
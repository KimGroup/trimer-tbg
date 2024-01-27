#include <chrono>
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
	}

	Sample(int32_t w, int32_t h, const std::vector<TrimerPos>& trimers) : w(w), h(h)
	{
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		for (auto& i : trimers)
			trimer_occupations[i.index(w)] = true;

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
		clusters_total = 0;
		j4_total = 0;

		for (const auto& occ : vertex_occupations)
		{
			auto popc = __builtin_popcount(occ.occupations);
			clusters_total += popc * popc - popc;
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

	int j4_neighbors_of(const TrimerPos& pos)
	{
		int pairs = 0;
		for (auto& rel : j4_neighbor_list[pos.s])
			if (trimer_occupations[TrimerPos(pos.x + rel.x, pos.y + rel.y, 1 - pos.s).canonical(w, h).index(w)])
				pairs++;

		return pairs;
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
		{
			for (int j = 0; j < h; j++)
			{
				if (vertex_occupations[MonomerPos(i, j).index(w)].occupations == 0)
				{
					r.emplace_back(i, j);
				}
			}
		}
		return r;
	}

	std::vector<MonomerPos> dimer_monomer_correlation() const
	{
		std::vector<MonomerPos> ret;

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
			{
				disjoint_monos.push_back(monos[i]);
			}
		}

		for (auto& i : dimers)
		{
			for (auto& j : disjoint_monos)
			{
				ret.push_back(j.center_at(i.first, w, h).rotate(-i.second).canonical(w, h));
			}
		}

		return ret;
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
				double u_factor = interactions.u > 0 ? u * interactions.u : 0;
				double j4_factor = interactions.j4 > 0 ? j4 * interactions.j4 : 0;

				if (u_factor + j4_factor == infinity || uniform(rng) < 1 - std::exp(-(u_factor + j4_factor)))
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
			trimer_occupations[i.index(w)] = true;

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

void sim_0k()
{
	std::minstd_rand rng;
	rng.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				 .count());

	int sis[5] = {6, 12, 24, 48, 96};

	for (int si = 0; si < 5; si++)
	{
		int s = sis[si];
		std::vector<TrimerPos> pos;
		for (int i = 0; i < s; i += 3)
		{
			for (int j = 0; j < s; j += 3)
			{
				if (i > 0 || j > 0)
				{
					pos.emplace_back(i, j, 0);
				}
				pos.emplace_back(i + 1, j + 1, 0);
				pos.emplace_back(i + 2, j + 2, 0);
			}
		}

		int N = 200000000;
		int stride = 50;

		std::stringstream ss;

		ss << "data/" << s << "x" << s << "-3-200000000.50-monomers.dat";
		std::ofstream of1(ss.str());

		// ss.str("");
		// ss << "data/" << s << "x" << s << "-3-1000000.1000-trimers-cut.dat";
		// std::ofstream of2(ss.str());

		// ss.str("");
		// ss << "data/" << s << "x" << s << "-3-1000000.1000-trimers.dat";
		// std::ofstream of3(ss.str());

		// ss.str("");
		// ss << "data/" << s << "x" << s << "-6-10000000-mono-di.dat";
		// std::ofstream of4(ss.str());

		auto sample = Sample(s, s, pos);
		for (int i = 0; i < N; i++)
		{
			sample.pocket_move(rng, infinity, 0);

			if (i % 1000 == 0)
			{
				std::cout << s << " " << i << std::endl;
			}

			if ((i + 1) % stride != 0)
				continue;

			// monomer positions
			auto mono = sample.all_monomers();
			for (uint j = 0; j < mono.size(); j++)
			{
				of1 << mono[j] << " ";
			}
			of1 << std::endl;

			// trimer cut
			// for (int j = 1; j <= s / 2; j++)
			// {
			// 	of2 << sample.trimer_correlation(TrimerPos(j, 0, 0)) << " ";
			// }
			// of2 << std::endl;

			// trimer positions
			// for (size_t j = 0; j < sample.trimer_occupations.size(); j++)
			// {
			// 	if (sample.trimer_occupations[j])
			// 	{
			// 		of3 << TrimerPos::from_index(j, sample.w) << " ";
			// 	}
			// }
			// of3 << std::endl;

			// mono-di
			// auto mono_di = sample.dimer_monomer_correlation();
			// for (auto& i : mono_di)
			// {
			// 	of << i << " ";
			// }
			// of << std::endl;
		}
	}
}

void sim_T()
{
	std::minstd_rand rng;
	rng.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				 .count());

	int sis[5] = {6, 12, 24, 48, 96};

	for (int si = 0; si < 5; si++)
	{
		int s = sis[si];
		std::vector<TrimerPos> pos;
		for (int i = 0; i < s; i += 3)
		{
			for (int j = 0; j < s; j += 3)
			{
				if (i > 0 || j > 0)
				{
					pos.emplace_back(i, j, 0);
				}
				pos.emplace_back(i + 1, j + 1, 0);
				pos.emplace_back(i + 2, j + 2, 0);
			}
		}

		auto sample = Sample(s, s, pos);

		int N = 10000;
		int stride = 1;

		double u = infinity;
		double j4 = 0;

		std::stringstream ss;
		ss << "data/FT/" << s << "x" << s << "/r-3_u" << std::fixed << std::setprecision(2) << u << "_4j" << j4 << "_"
		   << N << "." << stride << "-";

		std::ofstream of0(ss.str() + "positions.dat");

		for (int i = 0; i < N; i++)
		{
			sample.pocket_move(rng, u, j4);

			if (i % 1000 == 0)
			{
				std::cout << s << " " << i << std::endl;
			}

			if ((i + 1) % stride != 0)
				continue;

			// trimer positions
			for (size_t j = 0; j < sample.trimer_occupations.size(); j++)
			{
				if (sample.trimer_occupations[j])
				{
					of0 << TrimerPos::from_index(j, sample.w) << " ";
				}
			}
			of0 << std::endl;
		}
	}
}

void test_energy()
{
	std::vector<TrimerPos> pos;
	for (int i = 0; i < 6; i += 3)
		for (int j = 0; j < 6; j += 3)
		{
			pos.emplace_back(i, j, 0);
			pos.emplace_back(i + 1, j + 1, 0);
			pos.emplace_back(i + 2, j + 2, 0);
		}

	auto sample = Sample(6, 6, pos);
	TEST_ASSERT(sample.cluster_energy == 0)
	TEST_ASSERT(sample.j4_pairs == 0)

	for (int i = 0; i < 6; i += 3)
		for (int j = 0; j < 6; j += 3)
		{
			pos.emplace_back(i + 1, j, 0);
		}

	sample = Sample(6, 6, pos);
	TEST_ASSERT(sample.cluster_energy == 24)
	TEST_ASSERT(sample.j4_pairs == 0)
}

void test_metro()
{
	std::vector<TrimerPos> pos;
	for (int i = 0; i < 6; i += 3)
		for (int j = 0; j < 6; j += 3)
		{
			pos.emplace_back(i, j, 0);
			pos.emplace_back(i + 1, j + 1, 0);
			pos.emplace_back(i + 2, j + 2, 0);
		}

	PTEnsemble ensemble;
	ensemble.j4 = 1;
	ensemble.chains.emplace_back(PTEnsemble::Chain{1, Sample(6, 6, pos)});

	std::minstd_rand rng;
	rng.seed(1234);

	for (int i = 0; i < 1000; i++)
	{
		ensemble.step_all(rng);
	}
}

void test_pocket()
{
	std::minstd_rand rng;
	rng.seed(1234);

	std::vector<TrimerPos> pos;
	for (int i = 0; i < 6; i += 3)
	{
		for (int j = 0; j < 6; j += 3)
		{
			if (i > 0 || j > 0)
			{
				pos.emplace_back(i, j, 0);
			}
			pos.emplace_back(i + 1, j + 1, 0);
			pos.emplace_back(i + 2, j + 2, 0);
		}
	}

	auto sample = Sample(6, 6, pos);

	int test[8] = {355, 382, 427, 381, 384, 360, 344, 355};
	for (int it = 0; it < 8; it++)
	{
		for (int i = 0; i < 100; i++)
			sample.pocket_move(rng, 0, infinity);

		int total = 0;
		for (size_t i = 0; i < sample.trimer_occupations.size(); i++)
		{
			if (sample.trimer_occupations[i])
				total += i;
		}

		if (total != test[it])
		{
			TEST_ASSERT(false);
			return;
		}
	}

	TEST_ASSERT(true);
}

void test()
{
	test_energy();
	test_pocket();
	test_metro();
}

int main()
{
#ifdef TEST
	test();
#else
	sim_T();
#endif
}
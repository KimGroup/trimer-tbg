#include <chrono>
#include <fstream>
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
#else
#define ASSERT(x)
#endif

int32_t pmod(int32_t a, int32_t b) { return (a % b + b) % b; }

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
	TrimerPos find_first_occupation(MonomerPos pos, int32_t w, int32_t h) const;
	static Cluster relative(int dx, int dy, int s)
	{
		if (dx != 0)
			dx = 1;
		if (dy != 0)
			dy = 1;

		return Cluster{(uint8_t)(1 << (dx * 4 + dy * 2 + s))};
	}
};

struct TrimerPos
{
	int32_t x, y;
	int8_t s;

	TrimerPos() : x(0), y(0), s(0) {}
	TrimerPos(int32_t x, int32_t y, int8_t s) : x(x), y(y), s(s) {}

	MonomerPos mono_pos() const { return MonomerPos(x, y); }

	std::array<MonomerPos, 3> get_occupations(int32_t w, int32_t h) const
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

	std::array<std::pair<MonomerPos, Cluster>, 3> get_occupations_wrel(int32_t w, int32_t h) const
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

TrimerPos Cluster::find_first_occupation(MonomerPos pos, int32_t w, int32_t h) const
{
	for (int dx = -1; dx <= 0; dx++)
		for (int dy = -1; dy <= 0; dy++)
			for (int s = 0; s < 2; s++)
				if (occupations & relative(dx, dy, s).occupations)
					return TrimerPos(pos.x + dx, pos.y + dy, s).canonical(w, h);

	ASSERT(false);
	return {0, 0, 2};
}

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
	int32_t w, h;

	std::vector<bool> trimer_occupations;
	std::vector<Cluster> vertex_occupations;
	float energy;

	Sample(int32_t w, int32_t h) : w(w), h(h)
	{
		energy = 0;
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		vertex_occupations = std::vector<Cluster>(w * h);
	}

	Sample(int32_t w, int32_t h, const std::vector<TrimerPos>& trimers) : w(w), h(h)
	{
		energy = 0;
		trimer_occupations = std::vector<bool>(w * h * 2, false);
		for (auto& i : trimers)
			trimer_occupations[i.index(w)] = true;

		regenerate_occupation();
	}

	void regenerate_occupation()
	{
		vertex_occupations = std::vector<Cluster>(w * h);

		for (size_t i = 0; i < trimer_occupations.size(); i++)
		{
			if (trimer_occupations[i])
			{
				auto tri = TrimerPos::from_index(i, w);

				for (auto& [occ, rel] : tri.get_occupations_wrel(w, h))
					vertex_occupations[occ.index(w)].occupations |= rel.occupations;
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

	template <typename Rng> void pocket_move(Rng& rng)
	{
		TrimerPos seed(0, 0, 2);
		while (seed.s == 2)
		{
			uint32_t candidate = rng() % trimer_occupations.size();
			if (trimer_occupations[candidate])
			{
				seed = TrimerPos::from_index(candidate, w);
			}
		}

		std::vector<TrimerPos> pocket{seed}, Abar;
		std::vector<std::pair<MonomerPos, TrimerPos>> Abar_entries;

		trimer_occupations[seed.index(w)] = false;
		for (auto& i : seed.get_occupations(w, h))
		{
			ASSERT(vertex_occupations[i.index(w)].occupations > 0)
			vertex_occupations[i.index(w)].occupations = 0;
		}

		auto symc = MonomerPos(rng() % w, rng() % h);
		auto syma = rng() % 6;

		while (pocket.size() > 0)
		{
			auto el = pocket[rng() % pocket.size()];
			auto moved = el.reflect(symc, syma, w, h);

			pocket.erase(std::find(pocket.begin(), pocket.end(), el));
			Abar.push_back(moved);

			for (auto& i : moved.get_occupations(w, h))
			{
				Abar_entries.push_back(std::make_pair(i, moved));

				if (vertex_occupations[i.index(w)].occupations > 0)
				{
					auto overlap = vertex_occupations[i.index(w)].find_first_occupation(i, w, h);
					trimer_occupations[overlap.index(w)] = false;

					pocket.push_back(overlap);
					for (auto& j : overlap.get_occupations(w, h))
					{
						ASSERT(vertex_occupations[j.index(w)].occupations > 0)
						vertex_occupations[j.index(w)].occupations = 0;
					}
				}
			}
		}

		for (auto& i : Abar)
			trimer_occupations[i.index(w)] = true;

		for (auto& pair : Abar_entries)
		{
			ASSERT(vertex_occupations[pair.first.index(w)].occupations == 0)
			vertex_occupations[pair.first.index(w)] =
				Cluster::relative(pair.second.x - pair.first.x, pair.second.y - pair.first.y, pair.second.s);
		}
	}
};

void sim()
{
	std::minstd_rand rng;
	rng.seed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
				 .count());

	int sis[5] = {6, 12, 24, 48, 96};

	// for (int s = 12; s < 108; s += 12)
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
			sample.pocket_move(rng);

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

void test_cluster()
{
	std::cout << Cluster::relative(-1, -1, 1).find_first_occupation({0, 0}, 6, 6) << std::endl;
	std::cout << Cluster::relative(-1, 0, 1).find_first_occupation({0, 0}, 6, 6) << std::endl;
	std::cout << Cluster::relative(0, -1, 1).find_first_occupation({0, 0}, 6, 6) << std::endl;
	std::cout << Cluster::relative(0, 0, 0).find_first_occupation({0, 0}, 6, 6) << std::endl;
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

	int test[8] = {348, 428, 399, 341, 393, 380, 359, 347};
	for (int it = 0; it < 8; it++)
	{
		for (int i = 0; i < 100; i++)
			sample.pocket_move(rng);

		int total = 0;
		for (size_t i = 0; i < sample.trimer_occupations.size(); i++)
		{
			if (sample.trimer_occupations[i])
				total += i;
		}

		if (total != test[it])
		{
			std::cout << "pocket test failed" << std::endl;
			return;
		}
	}
	std::cout << "pocket test passed" << std::endl;
}

void test()
{
	test_cluster();
	test_pocket();
}

int main()
{
#ifdef TEST
	test();
#else
	sim();
#endif
}
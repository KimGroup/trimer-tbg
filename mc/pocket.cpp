#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

	MonomerPos operator-(const MonomerPos& other) const { return MonomerPos(x - other.x, y - other.y); }
	MonomerPos operator+(const MonomerPos& other) const { return MonomerPos(x + other.x, y + other.y); }
	bool operator==(const MonomerPos& other) const { return x == other.x && y == other.y; }
};

struct TrimerPos
{
	int32_t x, y;
	int8_t s;

	TrimerPos() : x(0), y(0), s(0) {}
	TrimerPos(int32_t x, int32_t y, int8_t s) : x(x), y(y), s(s) {}

	MonomerPos mono_pos() const { return MonomerPos(x, y); }

	std::vector<MonomerPos> get_occupations(int32_t w, int32_t h) const
	{
		std::vector<MonomerPos> r;

		if (s == 0)
			r = std::vector<MonomerPos>{{x, y}, {x + 1, y}, {x, y + 1}};
		else
			r = std::vector<MonomerPos>{{x + 1, y + 1}, {x + 1, y}, {x, y + 1}};

		for (auto& i : r)
			i = i.canonical(w, h);

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

std::minstd_rand rng;
struct Sample
{
	int32_t w, h;

	std::vector<TrimerPos> trimers;
	std::unordered_map<MonomerPos, TrimerPos> occupations;

	Sample(int32_t w, int32_t h) : w(w), h(h) {}

	Sample(int32_t w, int32_t h, const std::vector<TrimerPos>& trimers) : w(w), h(h), trimers(trimers)
	{
		regenerate_occupation();
	}
	Sample(int32_t w, int32_t h, std::vector<TrimerPos>&& trimers) : w(w), h(h), trimers(trimers)
	{
		regenerate_occupation();
	}

	void regenerate_occupation()
	{
		occupations.clear();
		for (auto& i : trimers)
		{
			for (auto& occ : i.get_occupations(w, h))
			{
				occupations[occ] = i;
			}
		}
	}

	double trimer_correlation(TrimerPos d) const
	{
		std::unordered_set<TrimerPos> tris(trimers.begin(), trimers.end());

		int total = 0;
		int found = 0;

		for (auto& p1 : tris)
		{
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

			if (tris.contains(n.canonical(w, h)))
				found++;
			total++;
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
				if (!occupations.contains(MonomerPos(i, j)))
				{
					r.emplace_back(i, j);
				}
			}
		}
		return r;
	}
};

Sample pocket_move(Sample&& s)
{
	auto seed = s.trimers[rng() % s.trimers.size()];

	std::vector<TrimerPos> pocket{seed}, Abar;
	std::unordered_map<MonomerPos, TrimerPos> Abar_occ;

	s.trimers.erase(std::find(s.trimers.begin(), s.trimers.end(), seed));
	for (auto& i : seed.get_occupations(s.w, s.h))
	{
		s.occupations.erase(i);
	}

	auto symc = MonomerPos(rng() % s.w, rng() % s.h);
	auto syma = rng() % 6;

	while (pocket.size() > 0)
	{
		auto el = pocket[rng() % pocket.size()];
		auto moved = el.reflect(symc, syma, s.w, s.h);

		pocket.erase(std::find(pocket.begin(), pocket.end(), el));
		Abar.push_back(moved);

		for (auto& i : moved.get_occupations(s.w, s.h))
		{
			Abar_occ[i] = moved;

			if (s.occupations.contains(i))
			{
				auto overlap = s.occupations[i];
				s.trimers.erase(std::find(s.trimers.begin(), s.trimers.end(), overlap));

				pocket.push_back(overlap);
				for (auto& j : overlap.get_occupations(s.w, s.h))
				{
					s.occupations.erase(j);
				}
			}
		}
	}

	Sample new_sample(s.w, s.h);
	new_sample.trimers = std::move(s.trimers);
	new_sample.trimers.insert(new_sample.trimers.end(), Abar.begin(), Abar.end());
	new_sample.occupations = std::move(s.occupations);
	new_sample.occupations.insert(Abar_occ.begin(), Abar_occ.end());
	return new_sample;
}

int main()
{
	for (int s = 108; s < 109; s += 6)
	{
		std::vector<TrimerPos> pos;
		for (int i = 0; i < s; i += 3)
		{
			for (int j = 0; j < s; j += 3)
			{
				if (i > 0 || j > 0)
					pos.emplace_back(i, j, 0);
				pos.emplace_back(i + 1, j + 1, 0);
				pos.emplace_back(i + 2, j + 2, 0);
			}
		}

		std::stringstream ss;
		// ss << "data/" << s << "x" << s << "-3-500000-monomers.dat";
		// std::ofstream of(ss.str());

		ss << "data/" << s << "x" << s << "-3-20000000-trimers-cut.dat";
		std::ofstream of(ss.str());

		// ss << "data/" << s << "x" << s << "-3-1000-trimers.dat";
		// std::ofstream of(ss.str());

		auto sample = Sample(s, s, pos);
		for (int i = 0; i < 20000000; i++)
		{
			sample = pocket_move(std::move(sample));
			if (i % 1000 == 0)
			{
				std::cout << s << " " << i << std::endl;
			}

			// monomer positions
			// auto mono = sample.all_monomers();
			// for (uint j = 0; j < mono.size(); j++)
			// {
			// 	of << mono[j];
			// }
			// of << std::endl;

			// trimer cut
			for (int j = 1; j <= s / 2; j++)
			{
				of << sample.trimer_correlation(TrimerPos(j, 0, 0)) << " ";
			}
			of << std::endl;

			// trimer positions
			// for (auto& j : sample.trimers)
			// {
			// 	of << j << " ";
			// }
			// of << std::endl;
		}
	}
}
#pragma once

#include <ostream>
#include <tuple>
#include <vector>
#include <array>

int32_t pmod(int32_t a, int32_t b)
{
	int32_t m = a % b;
	if (m < 0) m += b;
	return m;
}

struct LatticePos
{
    static constexpr int8_t unit_cell_size() { return 1; }

	int32_t x, y;

	LatticePos() : x(0), y(0) {}
	LatticePos(int32_t x, int32_t y) : x(x), y(y) {}

	uint32_t index(int w) const { return x + y * w; }
	static LatticePos from_index(uint32_t index, int w) { return LatticePos(index % w, index / w); }

	LatticePos operator+(const LatticePos& other) const { return LatticePos(x + other.x, y + other.y); }
	LatticePos operator-(const LatticePos& other) const { return LatticePos(x - other.x, y - other.y); }
	LatticePos operator-() const { return LatticePos(-x, -y); }
	bool operator==(const LatticePos& other) const { return x == other.x && y == other.y; }
};

template<int8_t UnitCellSize>
struct SublatticePos
{
    static constexpr int8_t unit_cell_size() { return UnitCellSize; }

	int32_t x, y;
	int8_t s;

	SublatticePos() : x(0), y(0), s(0) {}
	SublatticePos(int x, int y, int8_t s) : x(x), y(y), s(s) {}

	LatticePos lattice_pos() const { return LatticePos(x, y); }

	uint32_t index(int w) const { return (x + y * w) * unit_cell_size() + s; }
	static SublatticePos from_index(uint32_t index, int w)
	{
		uint32_t half = index / unit_cell_size();
		return SublatticePos(half % w, half / w, (int8_t) (index % unit_cell_size()));
	}

	bool operator==(const SublatticePos& other) const { return x == other.x && y == other.y && s == other.s; }
	SublatticePos operator+(const LatticePos& other) const { return SublatticePos(x + other.x, y + other.y, s); }
	SublatticePos operator-(const LatticePos& other) const { return SublatticePos(x - other.x, y - other.y, s); }
};

std::ostream& operator<<(std::ostream& o, const LatticePos& v)
{
    return o << "(" << v.x << "," << v.y << ")";
}

template<int8_t UnitCellSize>
std::ostream& operator<<(std::ostream& o, const SublatticePos<UnitCellSize>& v)
{
	return o << "(" << v.x << "," << v.y << "," << (int)v.s << ")";
}

namespace std
{
template<> struct hash<LatticePos>
{
	std::size_t operator()(const LatticePos& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
template<int8_t UnitCellSize> struct hash<SublatticePos<UnitCellSize>>
{
	std::size_t operator()(const SublatticePos<UnitCellSize>& x) const
	{
		std::size_t seed = std::hash<int32_t>()(x.x);
		seed ^= std::hash<int32_t>()(x.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int32_t>()(x.s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
}

struct PeriodicBoundaryCondition
{
    int w, h;
    PeriodicBoundaryCondition(int w, int h) : w(w), h(h) { }

    LatticePos principal(const LatticePos& pos) const { return LatticePos(pmod(pos.x, w), pmod(pos.y, h)); }
	template<int8_t n>
    SublatticePos<n> principal(const SublatticePos<n>& pos) const { return SublatticePos<n>(pmod(pos.x, w), pmod(pos.y, h), pos.s); }

	LatticePos center(const LatticePos& pos, const LatticePos& center) const
	{
		return LatticePos(pmod(pos.x - center.x + w / 2, w) - w / 2, pmod(pos.y - center.y + h / 2, h) - h / 2);
	}
	template<int8_t n>
	SublatticePos<n> center(const SublatticePos<n>& pos, const LatticePos& center) const
	{
		return SublatticePos<n>(pmod(pos.x - center.x + w / 2, w) - w / 2, pmod(pos.y - center.y + h / 2, h) - h / 2, pos.s);
	}
};

struct SkewBoundaryCondition
{
    int w, h;
    SkewBoundaryCondition(int w, int h) : w(w), h(h) { }

    LatticePos principal(const LatticePos& pos) const
	{
		auto copy = pos;
		while (copy.y >= h)
		{
			copy.x -= h;
			copy.y -= h;
		}
		while (copy.y < 0)
		{
			copy.x += h;
			copy.y += h;
		}

		copy.x = pmod(copy.x, w);
		return copy;
	}

	template<int8_t n>
    SublatticePos<n> principal(const SublatticePos<n>& pos) const
    {
        auto lat = principal(pos.lattice_pos());
        return SublatticePos<n>(lat.x, lat.y, pos.s);
    }

	LatticePos center(const LatticePos& pos, const LatticePos& center) const
	{
		return pos - center;
	}
	template<int8_t n>
	SublatticePos<n> center(const SublatticePos<n>& pos, const LatticePos& center) const
	{
		return pos - center;
	}
};

struct TriangularGeometry
{
	LatticePos reflect(const LatticePos& pos, int dir) const
	{
		switch (dir)
		{
            default:
            case 0:
                return LatticePos(pos.x + pos.y, -pos.y);
            case 1:
                return LatticePos(pos.y, pos.x);
            case 2:
                return LatticePos(-pos.x, pos.x + pos.y);
            case 3:
                return LatticePos(-pos.x - pos.y, pos.y);
            case 4:
                return LatticePos(-pos.y, -pos.x);
            case 5:
                return LatticePos(pos.x, -pos.x - pos.y);
		}
	}

	LatticePos rotate(const LatticePos& pos, int amount) const
	{
		switch (amount)
		{
            default:
            case 0:
                return LatticePos(pos.x, pos.y);
            case 1:
                return LatticePos(-pos.y, pos.x + pos.y);
            case 2:
                return LatticePos(-pos.x - pos.y, pos.x);
            case 3:
                return LatticePos(-pos.x, -pos.y);
            case 4:
                return LatticePos(pos.y, -pos.x - pos.y);
            case 5:
                return LatticePos(pos.x + pos.y, -pos.x);
		}
	}
};

template<typename BoundaryCondition = PeriodicBoundaryCondition>
struct TrimerTriangularGeometry : public TriangularGeometry, public BoundaryCondition
{
    using vertex_t = LatticePos;
    using bond_t = SublatticePos<2>;

    TrimerTriangularGeometry(int w, int h) : BoundaryCondition(w, h) { }

    std::array<std::array<bond_t, 6>, 2> rotations = {
        std::array<bond_t, 6>{ bond_t{0, 0, 0}, {-1, 0, 1}, {-1, 0, 0}, {-1, -1, 1}, {0, -1, 0}, {0, -1, 1} },
        std::array<bond_t, 6>{ bond_t{0, 0, 1}, {-1, 1, 0}, {-2, 0, 1}, {-1, -1, 0}, {0, -2, 1}, {1, -1, 0} },
    };

    using TriangularGeometry::rotate;
	// rotate around the plaquette at (0, 0)
	bond_t rotate(const bond_t& pos, int amount) const
	{
        return rotations[pos.s][amount] + rotate(pos.lattice_pos(), amount);
	}

    std::array<std::array<bond_t, 6>, 2> reflections = {
        std::array<bond_t, 6>{ bond_t{0, -1, 1}, {0, 0, 0}, {-1, 0, 1}, {-1, 0, 0}, {-1, -1, 1}, {0, -1, 0} },
        std::array<bond_t, 6>{ bond_t{1, -1, 0}, {0, 0, 1}, {-1, 1, 0}, {-2, 0, 1}, {-1, -1, 0}, {0, -2, 1} },
    };

    using TriangularGeometry::reflect;
    // reflect about the plaquette (0, 0)
	bond_t reflect(const bond_t& pos, int dir) const
	{
        return reflections[pos.s][dir] + reflect(pos.lattice_pos(), dir);
	}

    constexpr int n_symmetries() const { return 20; }

	bond_t apply_symmetry(const bond_t& pos, const LatticePos& c, int index) const
	{
		auto p = this->center(pos, c);

		if (index < 12)
			p = reflect(p, index % 6);
		else if (index < 17)
			p = rotate(p, index % 6 + 1);
		else if (index == 17)
			// pi rotation around bond 1
			p = bond_t(-p.x, -p.y, (int8_t)(1-p.s));
		else if (index == 18)
			// 2pi/3 around sublattice A
			p = bond_t(-p.x-p.y-p.s, p.x, p.s);
		else if (index == 19)
			// -2pi/3 around sublattice A
			p = bond_t(p.y, -p.x-p.y-p.s, p.s);

		return this->principal(p + c);
	}

	std::array<vertex_t, 3> get_vertices(const bond_t& pos) const
	{
		std::array<vertex_t, 3> r;

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

		r[0] = this->principal(r[0]);
		r[1] = this->principal(r[1]);
		r[2] = this->principal(r[2]);

		return r;
	}

	std::array<bond_t, 6> vertex_trimers = {
		bond_t{0, 0, 0}, {-1, 0, 1}, {-1, 0, 0}, {-1, -1, 1}, {0, -1, 0}, {0, -1, 1},
	};

	std::array<bond_t, 6> get_bonds(const vertex_t& pos) const
	{
		std::array<bond_t, 6> r;

		for (int i = 0; i < 6; i++)
			r[i] = this->principal(vertex_trimers[i] + pos);

		return r;
	}

	std::array<std::array<vertex_t, 1>, 6> vertex_flips = {
		std::array<vertex_t, 1>{ vertex_t{1, 1} },
		std::array<vertex_t, 1>{ vertex_t{-1, 2} },
		std::array<vertex_t, 1>{ vertex_t{-2, 1} },
		std::array<vertex_t, 1>{ vertex_t{-1, -1} },
		std::array<vertex_t, 1>{ vertex_t{1, -2} },
		std::array<vertex_t, 1>{ vertex_t{2, -1} },
	};

	std::array<std::array<bond_t, 1>, 6> bond_flips = {
		std::array<bond_t, 1>{ bond_t{0, 0, 1} },
		std::array<bond_t, 1>{ bond_t{-1, 1, 0} },
		std::array<bond_t, 1>{ bond_t{-2, 0, 1} },
		std::array<bond_t, 1>{ bond_t{-1, -1, 0} },
		std::array<bond_t, 1>{ bond_t{0, -2, 1} },
		std::array<bond_t, 1>{ bond_t{1, -1, 0} },
	};

	auto get_flips(const vertex_t& vtx, int bond_index)
	{
		std::array<std::pair<vertex_t, bond_t>, 1> ret;
		ret[0] = std::make_pair(this->principal(vertex_flips[bond_index][0] + vtx), this->principal(bond_flips[bond_index][0] + vtx));

		return ret;
	}

	static constexpr bool has_j4() { return true; }

	std::array<std::array<bond_t, 6>, 2> j4_neighbor_list = {
		std::array<bond_t, 6>{ bond_t{0, 1, 1}, {1, 0, 1}, {0, -2, 1}, {1, -2, 1}, {-2, 0, 1}, {-2, 1, 1} },
		std::array<bond_t, 6>{ bond_t{0, 2, 0}, {-1, 2, 0}, {2, 0, 0}, {2, -1, 0}, {0, -1, 0}, {-1, 0, 0} },
    };
};

template<typename BoundaryCondition = PeriodicBoundaryCondition>
struct DimerHexagonalGeometry : public TriangularGeometry, public BoundaryCondition
{
    using vertex_t = SublatticePos<2>;
    using bond_t = SublatticePos<3>;

    DimerHexagonalGeometry(int w, int h) : BoundaryCondition(w, h) { }

	std::array<vertex_t, 2> get_vertices(const bond_t& pos) const
	{
		std::array<vertex_t, 2> r;

		r[0] = {pos.x, pos.y, 0};
		if (pos.s == 0)
			r[1] = {pos.x, pos.y, 1};
		else if (pos.s == 1)
			r[1] = {pos.x - 1, pos.y, 1};
		else
			r[1] = {pos.x, pos.y - 1, 1};

		r[0] = this->principal(r[0]);
		r[1] = this->principal(r[1]);

		return r;
	}

	std::array<std::array<bond_t, 3>, 2> vertex_dimers = {
		std::array<bond_t, 3>{ bond_t{0, 0, 0}, {0, 0, 1}, {0, 0, 2} },
		std::array<bond_t, 3>{ bond_t{0, 1, 2}, {0, 0, 0}, {1, 0, 1} },
	};

	std::array<bond_t, 3> get_bonds(const vertex_t& pos) const
	{
		std::array<bond_t, 3> r;

		for (int i = 0; i < 3; i++)
			r[i] = this->principal(vertex_dimers[pos.s][i] + pos.lattice_pos());

		return r;
	}

	std::array<std::array<bond_t, 6>, 3> reflections = {
		std::array<bond_t, 6>{ bond_t{1, -1, 1}, {0, 0, 0}, {-1, 1, 2}, {-1, 0, 1}, {-1, -1, 0}, {0, -1, 2} },
		std::array<bond_t, 6>{ bond_t{0, -1, 0}, {0, 0, 2}, {0, 0, 1}, {-1, 0, 0}, {-1, 0, 2}, {0, -1, 1} },
		std::array<bond_t, 6>{ bond_t{0, 0, 2}, {0, 0, 1}, {-1, 0, 0}, {-1, 0, 2}, {0, -1, 1}, {0, -1, 0} },
	};

    using TriangularGeometry::reflect;
	bond_t reflect(const bond_t& pos, int dir) const
	{
        return reflections[pos.s][dir] + reflect(pos.lattice_pos(), dir);
	}

    constexpr int n_symmetries() const { return 6; }

	bond_t apply_symmetry(const bond_t& pos, const LatticePos& c, int index) const
	{
		auto p = pos - c;

        p = reflect(p, index);

		return this->principal(p + c);
	}

	std::array<std::array<std::array<vertex_t, 2>, 3>, 2> vertex_flips = {
		std::array<std::array<vertex_t, 2>, 3>{
			std::array<vertex_t, 2>{ vertex_t{0, 1, 0}, {1, 0, 0} },
			std::array<vertex_t, 2>{ vertex_t{-1, 0, 0}, {-1, 1, 0} },
			std::array<vertex_t, 2>{ vertex_t{1, -1, 0}, {0, -1, 0} },
		},
		std::array<std::array<vertex_t, 2>, 3>{
			std::array<vertex_t, 2>{ vertex_t{-1, 1, 1}, {0, 1, 1} },
			std::array<vertex_t, 2>{ vertex_t{0, -1, 1}, {-1, 0, 1} },
			std::array<vertex_t, 2>{ vertex_t{1, 0, 1}, {1, -1, 1} },
		},
	};

	std::array<std::array<std::array<bond_t, 2>, 3>, 2> bond_flips = {
		std::array<std::array<bond_t, 2>, 3>{
			std::array<bond_t, 2>{ bond_t{0, 1, 2}, {1, 0, 1} },
			std::array<bond_t, 2>{ bond_t{-1, 0, 0}, {-1, 1, 2} },
			std::array<bond_t, 2>{ bond_t{1, -1, 1}, {0, -1, 0} },
		},
		std::array<std::array<bond_t, 2>, 3>{
			std::array<bond_t, 2>{ bond_t{0, 1, 1}, {0, 1, 0} },
			std::array<bond_t, 2>{ bond_t{0, 0, 2}, {0, 0, 1} },
			std::array<bond_t, 2>{ bond_t{1, 0, 0}, {1, 0, 2} },
		},
	};

	auto get_flips(const vertex_t& vtx, int bond_index)
	{
		std::array<std::pair<vertex_t, bond_t>, 2> ret;

		for (int i = 0; i < 2; i++)
			ret[i] = std::make_pair(
				this->principal(vertex_flips[vtx.s][bond_index][i] + vtx.lattice_pos()),
				this->principal(bond_flips[vtx.s][bond_index][i] + vtx.lattice_pos())
			);

		return ret;
	}

	static constexpr bool has_j4() { return false; }
};

template<typename BoundaryCondition = PeriodicBoundaryCondition>
struct DimerSquareGeometry : public BoundaryCondition
{
    using vertex_t = LatticePos;
    using bond_t = SublatticePos<2>;

    DimerSquareGeometry(int w, int h) : BoundaryCondition(w, h)
	{
		if (w != h)
		{
			std::cout << "square geometry doesn't work with different sized lattices";
			throw 0;
		}
	}

	LatticePos reflect(const LatticePos& pos, int dir) const
	{
		switch (dir)
		{
            default:
            case 0:
                return LatticePos(pos.x, -pos.y);
            case 1:
                return LatticePos(pos.y, pos.x);
            case 2:
                return LatticePos(-pos.x, pos.y);
            case 3:
                return LatticePos(-pos.y, -pos.x);
		}
	}

	std::array<std::array<bond_t, 4>, 2> reflections = {
		std::array<bond_t, 4>{ bond_t{0, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, -1, 1} },
		std::array<bond_t, 4>{ bond_t{0, -1, 1}, {0, 0, 0}, {0, 0, 1}, {-1, 0, 0} },
	};

	bond_t reflect(const bond_t& pos, int dir) const
	{
        return reflections[pos.s][dir] + reflect(pos.lattice_pos(), dir);
	}

    constexpr int n_symmetries() const { return 4; }

	bond_t apply_symmetry(const bond_t& pos, const LatticePos& c, int index) const
	{
		auto p = pos - c;

        p = reflect(p, index);

		return this->principal(p + c);
	}

	std::array<vertex_t, 2> get_vertices(const bond_t& pos) const
	{
		std::array<vertex_t, 2> r;
		r[0] = {pos.x, pos.y};

		if (pos.s == 0)
			r[1] = {pos.x + 1, pos.y};
		else
			r[1] = {pos.x, pos.y + 1};

		r[0] = this->principal(r[0]);
		r[1] = this->principal(r[1]);

		return r;
	}

	std::array<bond_t, 4> vertex_dimers = {
		bond_t{0, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, -1, 1}
	};

	std::array<bond_t, 4> get_bonds(const vertex_t& pos) const
	{
		std::array<bond_t, 4> r;

		for (int i = 0; i < 4; i++)
			r[i] = this->principal(vertex_dimers[i] + pos);

		return r;
	}

	std::array<std::array<vertex_t, 3>, 4> vertex_flips = {
		std::array<vertex_t, 3>{ vertex_t{1, 1}, {2, 0}, {1, -1} },
		std::array<vertex_t, 3>{ vertex_t{-1, 1}, {0, 2}, {1, 1} },
		std::array<vertex_t, 3>{ vertex_t{-1, -1}, {-2, 0}, {-1, 1} },
		std::array<vertex_t, 3>{ vertex_t{1, -1}, {0, -2}, {-1, -1} },
	};

	std::array<std::array<bond_t, 3>, 4> bond_flips = {
		std::array<bond_t, 3>{ bond_t{1, 0, 1}, {1, 0, 0}, {1, -1, 1} },
		std::array<bond_t, 3>{ bond_t{-1, 1, 0}, {0, 1, 1}, {0, 1, 0} },
		std::array<bond_t, 3>{ bond_t{-1, -1, 1}, {-2, 0, 0}, {-1, 0, 1} },
		std::array<bond_t, 3>{ bond_t{0, -1, 0}, {0, -2, 1}, {-1, -1, 0} },
	};

	auto get_flips(const vertex_t& vtx, int bond_index)
	{
		std::array<std::pair<vertex_t, bond_t>, 3> ret;

		for (int i = 0; i < 3; i++)
			ret[i] = std::make_pair(this->principal(vertex_flips[bond_index][i] + vtx), this->principal(bond_flips[bond_index][i] + vtx));

		return ret;
	}

	static constexpr bool has_j4() { return false; }
};
#ifndef FIELDB_H
#define FIELDB_H

using uint32 = std::uint32_t;

class Field3DB
{
    public:

    Field3DB(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz},data_(nx*ny*nz) {}

    inline double& operator()(uint32 const i, uint32 const j, uint32 const k)
    {
        return data_[ k + (j + i*ny_)*nz_ ];
    }

    inline double const& operator()(uint32 const i, uint32 const j, uint32 const k) const
    {
        return data_[ k + (j + i*ny_)*nz_ ];
    }

    std::string name() {return "contiguous vector";}

    std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}

    ~Field3DB() = default;


private:
    uint32 nx_, ny_, nz_;
    std::vector <double> data_;
};

#endif

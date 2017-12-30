#ifndef FIELDB_H
#define FIELDB_H

using uint32 = std::uint32_t;

class Field3DB
{
    public:

    Field3DB(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz}, nynz{ny*nz},data_(nx*ny*nz) {}

    double& operator()(uint32 i, uint32 j, uint32 k)
    {
        return data_[ k + j*nz_ + i*nynz ];
    }

    double const& operator()(uint32 i, uint32 j, uint32 k) const
    {
        return data_[ k + j*nz_ + i*nynz ];
    }

    std::string name() {return "contiguous vector";}

    std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}

    ~Field3DB() = default;


private:
    uint32 nx_, ny_, nz_;
    uint32 nynz;
    std::vector <double> data_;
};

#endif

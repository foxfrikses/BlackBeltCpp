#include <cstddef>

#include <array>
#include <unordered_map>

std::array<char, 4> symbols{'A', 'T', 'G', 'C'};
std::unordered_map<char, uint8_t> symbolToUint{
  {'A', 0},
  {'T', 1},
  {'G', 2},
  {'C', 3}
};


struct Nucleotide {
  char Symbol;
  size_t Position;
  int ChromosomeNum;
  int GeneNum;
  bool IsMarked;
  char ServiceInfo;
};


struct CompactNucleotide {
  uint32_t Position;
  uint16_t GeneNum : 15;
  uint16_t IsMarked : 1;
  uint8_t ChromosomeNum : 6;
  uint8_t Symbol : 2;
  char ServiceInfo;
};


bool operator == (const Nucleotide& lhs, const Nucleotide& rhs) {
  return (lhs.Symbol == rhs.Symbol)
         && (lhs.Position == rhs.Position)
         && (lhs.ChromosomeNum == rhs.ChromosomeNum)
         && (lhs.GeneNum == rhs.GeneNum)
         && (lhs.IsMarked == rhs.IsMarked)
         && (lhs.ServiceInfo == rhs.ServiceInfo);
}


CompactNucleotide Compress(const Nucleotide& n) {
  return CompactNucleotide{
      .Position = uint32_t (n.Position),
      .GeneNum = uint16_t(n.GeneNum),
      .IsMarked = uint16_t(n.IsMarked ? 1u : 0u),
      .ChromosomeNum = uint8_t(n.ChromosomeNum),
      .Symbol = symbolToUint[n.Symbol],
      .ServiceInfo = n.ServiceInfo
  };
};


Nucleotide Decompress(const CompactNucleotide& cn) {
  return Nucleotide{
    .Symbol = symbols[cn.Symbol],
    .Position = cn.Position,
    .ChromosomeNum = cn.ChromosomeNum,
    .GeneNum = cn.GeneNum,
    .IsMarked = (cn.IsMarked == 1),
    .ServiceInfo = cn.ServiceInfo
  };
}

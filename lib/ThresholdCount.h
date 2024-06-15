/*
 * Copyright (c) 2022, BullsEye
   Adopted from HayStack (PLDI 2019)
 */

#ifndef _ACCESS_H_
#define _ACCESS_H_

#include <isl/isl-noexceptions.h>
#include <map>
#include <string>
#include <vector>

#include "Definitions.h"
#include "Program.h"

// class holding the access information
class ThresholdCount {
public:
  ThresholdCount(const ThresholdCount &other) = default;
  ThresholdCount(machine_model model) : MachineModel_(model) {}

  // compute base cache miss information
  // void countCompulsoryMisses(isl::union_map First);
  void computeStackDistances();
  std::vector<long> countCapacityMisses(std::vector<long> CacheSizes, isl::pw_qpolynomial Count);
  // void computeAbstractDomain(piece);

  // compute the capacity misses for additional cache sizes
  std::vector<long> countCapacityMisses(std::vector<long> CacheSizes);

private:
  // helper methods
  piece createPiece(isl::set Domain, isl::qpolynomial Piece) const;
  void extractStackDistanceExpression(isl::pw_qpolynomial Count);
  void storeAffinePieces();

  void applyEqualization();
  void applyRasterization(isl::set Domain);

  // enumerate non-affine dimensions to count all points
  std::vector<int> findNonAffineDimensions(piece Piece) const;
  void enumerateNonAffineDimensions(piece Piece);

  // counting methods
  std::vector<long> countAffineDimensions(piece Piece,
                                          std::vector<long> Limits) const;
  std::vector<long> enumerateNonAffinePoints(piece Piece,
                                             std::vector<long> Limits) const;

  // helper functions to analyze piece and extract the affine expression
  bool isPieceAffine(piece Piece) const;
  isl::pw_aff extractAffineExpression(piece Piece) const;
  isl::aff extractAffineExpression(isl::qpolynomial, isl::set Domain,
                                   std::map<int, long> Values) const;
  long getPieceSize(piece &Piece) const;

  // elimination helper methods
  int computeExponent(piece Piece) const;
  int computeDimensionExponent(int Dimension, piece Piece) const;

  isl::qpolynomial
  computeReplacement(std::map<int, isl::qpolynomial> Replacements,
                     piece Piece) const;

  // methods to eliminate floor terms due to equalization
  std::vector<std::vector<std::tuple<int, long, long>>>
  findEqualizationCandidates(piece Piece) const;
  std::vector<long>
  computeSplits(std::vector<std::tuple<int, long, long>> Candidate,
                piece Piece) const;
  std::vector<piece>
  equalizeCandidate(std::vector<std::tuple<int, long, long>> Candidate,
                    std::vector<long> Splits, piece Piece) const;

  // methods to eliminate floor terms due to rasterization
  std::vector<int> findRasterDimensions(piece Piece) const;
  std::vector<isl::val> computeMultipliers(std::vector<int> Dimensions,
                                           piece Piece) const;
  std::vector<piece> rasterDimension(int Dimension, isl::val Multiplier,
                                     piece Piece) const;

  // method to verify splits
  bool verifySplit(piece Piece, std::vector<piece> Pieces) const;

  int threshold;

  machine_model MachineModel_;

  std::vector<piece> Expression_;

  // store the stack distance
  std::vector<piece> Affine_;
  std::vector<piece> NonAffine_;
  std::vector<std::pair<long, isl::val>> Constant_;
  std::vector<piece> NonAffine_approx_;
  std::vector<int> NonAffine_dims;
  std::vector<int> Affine_dims;
};

// // support sorting
// inline bool operator<(ThresholdCount const &A, ThresholdCount const &B) {
//   return A.getName() < B.getName();
// }

#endif

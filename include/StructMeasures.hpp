#pragma once

#include <vector>
#include <Measure.hpp>

using namespace std;

struct Measures{
    // Hits on each detector
    vector<Measure> lay1_particles;
    vector<Measure> lay2_particles;
    vector<Measure> lay3_particles;
    vector<Measure> lay4_particles;
    vector<Measure> lay5_particles;
    vector<Measure> lay6_particles;
    vector<Measure> lay7_particles;
    vector<Measure> lay8_particles;
  };
  struct MeasuresPointer {
    vector<Measure>* lay1_particles;
    vector<Measure>* lay2_particles;
    vector<Measure>* lay3_particles;
    vector<Measure>* lay4_particles;
    vector<Measure>* lay5_particles;
    vector<Measure>* lay6_particles;
    vector<Measure>* lay7_particles;
    vector<Measure>* lay8_particles;
  
    MeasuresPointer() {
      lay1_particles = new vector<Measure>();
      lay2_particles = new vector<Measure>();
      lay3_particles = new vector<Measure>();
      lay4_particles = new vector<Measure>();
      lay5_particles = new vector<Measure>();
      lay6_particles = new vector<Measure>();
      lay7_particles = new vector<Measure>();
      lay8_particles = new vector<Measure>();
    }
  
    ~MeasuresPointer() {
      delete lay1_particles;
      delete lay2_particles;
      delete lay3_particles;
      delete lay4_particles;
      delete lay5_particles;
      delete lay6_particles;
      delete lay7_particles;
      delete lay8_particles;
    }
  };
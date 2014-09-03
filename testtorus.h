#ifndef TESTTORUS_H
#define TESTTORUS_H


#include <parametrics/gmPTorus>


class TestTorus : public GMlib::PTorus<float> {
public:
  using PTorus::PTorus;

protected:
  void localSimulate(double dt) override {

    rotate( GMlib::Angle(90) * dt, GMlib::Vector<float,3>( 0.0f, 0.0f, 1.0f ) );
    rotate( GMlib::Angle(180) * dt, GMlib::Vector<float,3>( 1.0f, 1.0f, 0.0f ) );
  }

}; // END class TestTorus



#endif // TESTTORUS_H

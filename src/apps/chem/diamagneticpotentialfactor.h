/*
 * diamagneticpotentialfactor.h
 *
 *  Created on: 16 Apr 2019
 *      Author: fbischoff
 */

#ifndef SRC_APPS_CHEM_DIAMAGNETICPOTENTIALFACTOR_H_
#define SRC_APPS_CHEM_DIAMAGNETICPOTENTIALFACTOR_H_


#include <madness/mra/mra.h>
#include <chem/znemo.h>

namespace madness {


/// to be put in a separate file
class Diamagnetic_potential_factor {

public:

	/// constructor takes a world and the parameters for the calculation
	Diamagnetic_potential_factor(World& world, const Nemo_complex_Parameters& param)
		: world(world), param(param) {}

	/// return the diamagnetic factor
	real_function_3d factor() const {return diamagnetic_factor_;}

	/// return the square of the diamagnetic factor
	real_function_3d factor_square() const {return diamagnetic_factor_square;}

	/// return a custom factor for a given magnetic field
	real_function_3d custom_factor(const coord_3d& B, const coord_3d& v, const double extra_exponent=1.0) {
		const double absB=B.normf();
		const double& ee=extra_exponent;
		auto diamagnetic_HO = [&absB, &B, &v, &ee](const coord_3d& r) {
			coord_3d arg=0.5*cross(B,r)-v;
			return exp(-ee/absB*inner(arg,arg));
		};
		real_function_3d result=real_factory_3d(world).functor(diamagnetic_HO).truncate_on_project();
		return result;
	}

	/// recompute the factor and the potentials for given physical and explicit magnetic fields
	void recompute_functions(const double& pB, const double& eB, const std::vector<coord_3d>& v);

	/// apply the diamagnetic potential on rhs
	std::vector<complex_function_3d> apply_potential(const std::vector<complex_function_3d>& rhs) const;

private:
	World& world;				///< the world
	coord_3d physical_B={0,0,0};		///< the actual magnetic field strength
	coord_3d explicit_B={0,0,0};		///< the magnetic field strength encoded in the diamagnetic factor

	Nemo_complex_Parameters param;	///< the parameters for the magnetic field calculation

	/// the diamagnetic factor to cancel the diamagnetic potential
	real_function_3d diamagnetic_factor_;
	real_function_3d diamagnetic_factor_square;

	/// the boxed diamagnetic potential (for a given B)
	real_function_3d diamagnetic_boxed;

	/// compute the radius for the diamagnetic potential
	double compute_radius(const double B) const {
		const double height=param.diamagnetic_height();
		const double radius=sqrt(8.0*height/(B*B));
		return radius;
	}

	/// set physical and explict B, recompute the remaining potential
	void set_B_explicit_B(const coord_3d& pB, const coord_3d& eB) {
		physical_B=pB;
		explicit_B=eB;
		double diapot_diffB=sqrt(inner(pB,pB) - inner(eB,eB));
		print("setting B values: physical", physical_B, " explicit", explicit_B);
		double radius=compute_radius(diapot_diffB);
		print("computing radius to",radius);
	}
public:

};


} /* namespace madness */

#endif /* SRC_APPS_CHEM_DIAMAGNETICPOTENTIALFACTOR_H_ */

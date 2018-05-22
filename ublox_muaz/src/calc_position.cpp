
#include "ublox/calc_position.h"
#include "ublox/ublox_structures.h"
#include <cstring>
#include <iostream>
#include <cmath>
#include <time.h>
#define bPI                 3.1415926535898
#define bGM84               3.986005e14
#define bOMEGAE84           7.2921151467e-5

using namespace std;
using namespace ublox;

Position::Position(string name)
{
	this->name = name;
	this->coords.defined = false;
	for(int i=0;i<sizeof(this->dopplers)/sizeof(*(this->dopplers));i++)
	{
		this->dopplers[i] = 0.0;
		this->calcDopplers[i] = 0.0;
	}

}
string Position::getName()
{
	return this->name;
}
void Position::printInfo()
{
	cout<<this->name;
}

void Position::addEphemeris(ParsedEphemData ephemeris)
{
	this->ephemeris[ephemeris.prn] = ephemeris;
}
bool Position::ephemerisExists(uint32_t svid)
{
	if(this->ephemeris.find(svid) != this->ephemeris.end())
	{
		return true;
	}
	return false;
}
ParsedEphemData Position::getEphemeris(uint32_t id)
{
	return this->ephemeris[id];
}
ecef Position::calcPosition(int id,double _time)
{
	ParsedEphemData ephemeris = this->ephemeris[id];
	long double roota=			ephemeris.majaxis;	//                    =  5153.79589081;
	long double toe=			ephemeris.toe;	//                      =  93600.0;
	long double m0=				ephemeris.anrtime;	//                       =  1.05827953357;
	long double e=				ephemeris.ecc;		//                        =  0.00223578442819;
	long double delta_n=		ephemeris.dN;	//                  =  0.465376527657e-08;
	long double smallomega=		ephemeris.omega;//               =  2.06374037770;
	long double cus = 			ephemeris.cus;	//                      =  0.177137553692e-05; 
	long double cuc=			ephemeris.cuc;	//                     =  0.457651913166e-05; 
	long double crs=			ephemeris.crs;	//                      =  88.6875000000; 
	long double crc=			ephemeris.crc;	//                      =  344.968750000; 
	long double cis=			ephemeris.cis;	//                      = -0.856816768646e-07;
	long double cic=			ephemeris.cic;	//                     =  0.651925802231e-07; 
	long double idot=			ephemeris.dia;	//                     =  0.342514267094e-09; 
	long double i0=				ephemeris.ia;		//                       =  0.961685061380; 
	long double bigomega0=		ephemeris.wo;//                =  1.64046615454; 
	long double earthrate =  	bOMEGAE84;
	long double bigomegadot=	ephemeris.dwo;	//              = -0.856928551657e-08; 
	long double t = _time;	// 			86400.00;
	long double A;
	long double n0, n;
	long double tk;
	long double mk, ek, vk, tak, ik, omegak, phik, uk, rk;
	long double corr_u, corr_r, corr_i;
	long double xpk, ypk;
	long double xk, yk, zk;
	long double mkdot, ekdot, takdot, ukdot, ikdot, rkdot, omegakdot;
	long double xpkdot, ypkdot;
	long double xkdot, ykdot, zkdot;
	long iter;

	A = roota*roota;           //roota is the square root of A
	n0 = sqrt(bGM84/(A*A*A));  //bGM84 is what the ICD-200 calls Greek mu
	tk = t - toe;              //t is the time of the pos. & vel. request.
	n = n0 + delta_n;
	mk = m0 + n*tk;
	mkdot = n;
	ek = mk;
	for(iter=0; iter<7; iter++) ek = mk + e*sin(ek);  //Overkill for small e
	ekdot = mkdot/(1.0 - e*cos(ek));
	//In the line, below, tak is the true anomaly (which is nu in the ICD-200).
	tak = atan2( sqrt(1.0-e*e)*sin(ek), cos(ek)-e);
	takdot = sin(ek)*ekdot*(1.0+e*cos(tak))/(sin(tak)*(1.0-e*cos(ek)));

	phik = tak + smallomega;
	corr_u = cus*sin(2.0*phik) + cuc*cos(2.0*phik);
	corr_r = crs*sin(2.0*phik) + crc*cos(2.0*phik);
	corr_i = cis*sin(2.0*phik) + cic*cos(2.0*phik);
	uk = phik + corr_u;
	rk = A*(1.0-e*cos(ek)) + corr_r;
	ik = i0 + idot*tk + corr_i;

	ukdot = takdot +2.0*(cus*cos(2.0*uk)-cuc*sin(2.0*uk))*takdot;
	rkdot = A*e*sin(ek)*n/(1.0-e*cos(ek)) + 2.0*(crs*cos(2.0*uk)-crc*sin(2.0*uk))*takdot;
	ikdot = idot + (cis*cos(2.0*uk)-cic*sin(2.0*uk))*2.0*takdot;

	xpk = rk*cos(uk);
	ypk = rk*sin(uk);

	xpkdot = rkdot*cos(uk) - ypk*ukdot;
	ypkdot = rkdot*sin(uk) + xpk*ukdot;

	omegak = bigomega0 + (bigomegadot-earthrate)*tk - earthrate*toe;

	omegakdot = (bigomegadot-earthrate);

	xk = xpk*cos(omegak) - ypk*sin(omegak)*cos(ik);
	yk = xpk*sin(omegak) + ypk*cos(omegak)*cos(ik);
	zk =                   ypk*sin(ik);

	xkdot = ( xpkdot-ypk*cos(ik)*omegakdot )*cos(omegak)
	        - ( xpk*omegakdot+ypkdot*cos(ik)-ypk*sin(ik)*ikdot )*sin(omegak);
	ykdot = ( xpkdot-ypk*cos(ik)*omegakdot )*sin(omegak)
	        + ( xpk*omegakdot+ypkdot*cos(ik)-ypk*sin(ik)*ikdot )*cos(omegak);
	zkdot = ypkdot*sin(ik) + ypk*cos(ik)*ikdot;
	ecef c;
	c.defined = true;
	c.ecefX = xk;
	c.ecefY = yk;
	c.ecefZ = zk;
	c.ecefVX = xkdot;
	c.ecefVY = ykdot;
	c.ecefVZ = zkdot;
	return c;
}
double Position::calcDistance(ecef satPos)
{
	double distance = 0;
	if(this->coords.defined)
	{
		
		distance = sqrt(pow((satPos.ecefX - this->coords.ecefX),2)+pow((satPos.ecefY - this->coords.ecefY),2)+pow((satPos.ecefZ - this->coords.ecefZ),2));
	}
	return distance;
}
double Position::calcDoppler(int id, double _time)
{
	ecef t1 = this->calcPosition(id,_time); //satelite position
	t1.ecefX = this->coords.ecefX - t1.ecefX;
	t1.ecefY = this->coords.ecefY - t1.ecefY;
	t1.ecefZ = this->coords.ecefZ - t1.ecefZ;//user position - satelite position
	double doppler = (t1.ecefVX * t1.ecefX) + (t1.ecefVY * t1.ecefY) + (t1.ecefVZ * t1.ecefZ);//v dot (r_u - r_i)
	double d1 = this->calcDistance(t1);
	doppler = doppler/d1; //v dot (r_u - r_i)/|r_u - r_i|
	doppler = (doppler * 1575420000)/299792458; //v dot (r_u - r_i)/|r_u - r_i| * L/c
	//ecef t2 = this->calcPosition(id,_time+1);
	//double d2 = this->calcDistance(t2);
	//double velocity = d2-d1;
	//double doppler = 1575420000 * (1 - (velocity/299792458));
	//doppler =  doppler - 1575420000;
	return doppler;
}
void Position::setCoords(double x, double y, double z)
{
	this->coords.ecefX = x;
	this->coords.ecefY = y;
	this->coords.ecefZ = z;
	this->coords.defined = true;
}
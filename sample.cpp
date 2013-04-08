#include "plant.h"

#include <sstream>
#include <math.h>
#include <time.h>
#include <stdlib.h>

namespace AdaptiveTechniques {

class PlantImpl
{
private:
    static const std::string PARAM_SAMPLING_PERIOD;
    static const std::string PARAM_DISTURBANCES;

public:
    PlantImpl();

    double simulate(double input);

    void reset();

    void setParameter(const std::string &param, const std::string &value) throw (plant_error);

    std::string getParameter(const std::string &param) const throw (plant_error);

private:

    double m_q0max;   // maxsymalny dopływ

    double m_lasth1;  // poziom płynu w pierwszym zbiorniku
    double m_A1;      // powierzchnia pierwszego zbiornika
    double m_H1;      // wysokość maksymalna pierwszego zbiornika
    double m_k1;      // stała pierwszego zaworu

    double m_lastV2;  // objętość płynu w drugim zbiorniku
    double m_k2;      // stała drugiego zaworu
    double m_tga2;    // tga nachylenia stozka

    double m_speriod; // okres probkowania

    double m_disturbances; // zaklocenia
};

const std::string PlantImpl::PARAM_SAMPLING_PERIOD = "samplePeriod";
const std::string PlantImpl::PARAM_DISTURBANCES   = "disturbances";

PlantImpl::PlantImpl()
    : m_q0max(0.8627), m_lasth1(0), m_A1(0.7329), m_H1(4.6526), m_k1(0.2113), m_lastV2(0), m_k2(0.4336), m_tga2(4.2687), m_speriod(0.001), m_disturbances(0)
{
    srand (time(NULL));
}

// Prosta inercja
double PlantImpl::simulate(double input)
{
    if (input < 0) {
        input = 0;
    } else if (input > m_q0max) {
        input = m_q0max;
    }

    // dodanie zaklocen:
    double k1 = m_k1;
    double k2 = m_k2;
    if (m_disturbances){
        double random;
        // random 0.9  - 1.1  mocne na pierwszym zbiorniku
        random = 1 + 0.1  * 0.02 * ((rand() % 100) - 50) * m_disturbances;
        k1 = k1 * random;
        // random 0.98 - 1.02 slabe na drugim zbiorniku
        random = 1 + 0.02 * 0.02 * ((rand() % 100) - 50) * m_disturbances;
        k2 = k2 * random;
    }


    // strumień wejscieowy do pierwszego zbiornika:
    double q0 = input;

    // strumień wyjsciowy pierwszego zbiornika:
    double q1 = sqrt(m_lasth1) * k1;
    double q10 = 0;

    // zabezpieczenie przez wylaniem ponizej zera:
    if (q1 * m_speriod > m_lasth1 * m_A1) {
        q1 = m_lasth1 * m_A1 / m_speriod;
    }

    // aktualizacja wysokości w zbiorniku:
    double h1 = m_lasth1 + m_speriod * (q0 - q1) / m_A1;

    // przelew:
    if (h1 > m_H1) {
        // dodatkowy odpływ:
        q10 = (h1 - m_H1) * m_A1 / m_speriod;
        // odjecie dodatkowego odpływu (h1 będzxie wtedy równe H1
        h1 = h1 - m_speriod * q10 / m_A1;
        // dodanie dodatkowego odpływu do strumienia q1
        q1 += q10;
    }

    // obliczenie wysokości drugiego zbiornika:
    double h2 = pow( (m_lastV2 * 3 * m_tga2 * m_tga2 / M_PI), 1.0/3);

    // strumień wyjściowy drugiego zbiornika:
    double q2 = sqrt(h2) * k2;

    // zabezpieczenie przed minusowa wartościa:
    if (q2 * m_speriod > m_lastV2) {
        q2 = m_lastV2 / m_speriod;
    }

    // policzenie nowej objętości:
    double V2 = m_lastV2 + ( q1 - q2 ) * m_speriod;

    // listing stanów:
    // std::cout << "in:" << q0 << " q12:" << q1 << " over:" << q10 << " out:" << q2 << " h1:" << h1 << " h2:" << h2 << " V2:" << V2 << std::endl;

    // uaktualnienie stanów:
    m_lasth1 = h1;
    m_lastV2 = V2;

    // wyjscie:
    return q2;
}

void PlantImpl::reset()
{
    m_lasth1 = 0;
    m_lastV2 = 0;
}

void PlantImpl::setParameter(const std::string &param, const std::string &value)
    throw (plant_error)
{
    std::istringstream istr(value);
    double temp;

    istr >> temp;

    if(istr.bad())
        throw plant_error("Lipa: podana wartość w niewłaściwym formacie");

    if(param == PARAM_SAMPLING_PERIOD)
    {
        if(temp <= 0)
            throw plant_error("Lipa: okres próbkowania nie może być ujemny!");

        m_speriod = temp;
    }
    else if(param == PARAM_DISTURBANCES)
    {
        if(temp < 0 || temp > 1)
            throw plant_error("Lipa: zakłócenia od 0 do 1");

        m_disturbances = temp;
    }
    else
        throw plant_error("Lipa: nie ma takiego parametru :(");
}

std::string PlantImpl::getParameter(const std::string &param) const
    throw (plant_error)
{
    std::ostringstream ostr;

    if(param == PARAM_SAMPLING_PERIOD)
    {
        ostr << m_speriod;
    }
    else
        throw plant_error("Lipa: nie ma takiego parametru :(");

    return ostr.str();
}


//
// Implementacja metod klasy Plant (przekierowanie metod do klasy wewnętrznej)
//

Plant::Plant()
    : m_impl(new PlantImpl())
{
}

double Plant::simulate(double input)
{
    return m_impl->simulate(input);
}

void Plant::reset()
{
    m_impl->reset();
}

void Plant::setParameter(const std::string &param, const std::string &value)
    throw (plant_error)
{
    m_impl->setParameter(param, value);
}

std::string Plant::getParameter(const std::string &param) const
    throw (plant_error)
{
    return m_impl->getParameter(param);
}

}

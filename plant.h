/******************************************************************************
 *  _____________________________
 *  \_  ___________| \___________\
 *    \ \          |> \
 *     \ \echniki  | \ \daptacyjne
 *      \_\        |_|\_\
 *
 *  Komputerowe Systemy Sterowania
 *****************************************************************************/

#ifndef _G_PLANT_H
#define _G_PLANT_H

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#include <memory>
#include <string>
#include <stdexcept>

namespace AdaptiveTechniques
{

/**
    Klasa wyjątków rzucanych przez klasę Plant
*/
class plant_error : public std::runtime_error
{
public:
    explicit plant_error(const std::string& __arg)
        : std::runtime_error(__arg)
    {
    }
};

/**
    Klasa której implementację ukryto w kodzie źródłowym.
*/
class PlantImpl;

/**
    Klasa reprezentująca obiekt "rzeczywisty" SISO.
    Pełni rolę czarnej skrzynki z nieznaną zawartością.

    Podstawowa metoda interfejsu klasy to:
           ____________
    in    | simulate() |    out
    ----> |    ???     | ----->
          |____________|

    Ponadto istnieje możliwość przywrócenia obiektu do stanu
    początkowego oraz ustawiania/czytania parametrów dodatkowych.

    Instrukcja obsługi dla implementującego bibliotekę:
    - Przeczytać resztę dokumentacji tej klasy
    - Zajrzeć do przykładu sample.cpp
    - Podrapać się po głowie
    - Napisać własną klasę PlantImpl zgodnie z wytycznymi
    - Postarać się nie modyfikować tego pliku (plant.h)
    - Zbudować projekt
    - Sprawdzić 5x czy działa
    - Wysłać bibliotekę razem z plant.h drugiemu zespołowi
    - Wyjść z kumplami na piwo
*/
class DLL_PUBLIC Plant
{
public:

    /**
        Konstruktor domyślny.
        Inicjalizuje obiekt sterowania wedle woli implementującego.

        Uwagi dla implementującego:
        Obowiązkowo musi być utworzony obiekt PlantImpl, np. w liście
        inicjalizacyjnej konstruktora. Implementacja PlantImpl może być
        w zasadzie dowolna.
    */
    Plant();

    /**
        Metoda wykonująca jeden krok symulacji.

        Uwagi dla implementującego:
        Jeśli symulowany jest obiekt rzeczywisty, wypada aby wewnętrzna
        reprezentacja czasu zgadzała się jakoś z ogólnym pojmowaniem tej wielkości,
        tzn. jeśli np. metodą setParameter ustawiłem okres próbkowania na 0.001s,
        to jednokrotne wykonanie funkcji simulate() powinno zmienić stan obiektu tak,
        jakby faktycznie upłynął taki okres czasu.
        Jeśli symulowany jest obiekt ciągły nic nie stoi na przeszkodzie, by wykonywać
        tzw. małe kroki symulacji np. z okresem 0.00001s. Wówczas zwracamy wynik
        setnej iteracji "małej" pętli ukrytej wewnątrz funkcji simulate.
        Poza tą uwagą metoda symulacji jest całkowicie dowolna
        (byle uzasadniona matematycznie :)
    */
    double simulate(double);

    /**
        Metoda przywracająca obiekt do stanu początkowego.

        Uwagi dla implementującego:
    */
    void reset();

    /**
        Metoda dostępowa do wybranych parametrów modelu.

        Obowiązkowe parametry:
            - samplePeriod - okres próbkowania w sekundach

        Metoda rzuca wyjątek plant_error, jeśli podany parametr/wartość jest zła.
    */
    void setParameter(const std::string &param, const std::string &value) throw (plant_error);

    /**
        Metoda dostępowa do wybranych parametrów modelu (odczyt).

        Obowiązkowe parametry: patrz setParameter

        Metoda rzuca wyjątek plant_error, jeśli podany parametr jest zły.
    */
    std::string getParameter(const std::string &param) const throw (plant_error);

private:

    /** Kwintesencja obiektu regulacji :) */
    std::auto_ptr<PlantImpl> m_impl;
};


#endif // _G_PLANT_H

}

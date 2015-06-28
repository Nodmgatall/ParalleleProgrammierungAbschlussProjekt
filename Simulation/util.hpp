#ifndef UTIL_HPP
#define UTIL_HPP

/* * * * * * * * * * * * *
 * Velocity conversions  *
 * * * * * * * * * * * * */

/** Convert km/h to m/s */
template<typename TYPE>
TYPE kmh_to_mps(TYPE kmh) {
    return (kmh * 1000) / 3600;
}

/** Convert m/s to km/h */
template<typename TYPE>
TYPE mps_to_kmh(TYPE mps) {
    return ((double)mps / (double)1000) * 3600;
}

/** Convert km/h to km/s */
template<typename TYPE>
TYPE kmh_to_kms(TYPE kmh) {
    return kmh / 3600;
}

/** Convert km/s to km/h */
template<typename TYPE>
TYPE kms_to_kmh(TYPE kms) {
    return kms * 3600;
}

/** Convert km/h to mph */
template<typename TYPE>
TYPE kmh_to_mph(TYPE kmh) {
    return kmh / (long double) 1.609344;
}

/** Convert mph to km/h */
template<typename TYPE>
TYPE mph_to_kmh(TYPE mph) {
    return mph * (long double) 1.609344;
}


/* * * * * * * * * * * * *
 * Distance conversions  *
 * * * * * * * * * * * * */

/** Convert kilometers to miles */
template<typename TYPE>
TYPE km_to_m(TYPE km) {
    return km * (long double) 0.62137;
}

/** Convert miles to kilometers */
template<typename TYPE>
TYPE m_to_km(TYPE miles) {
    return miles / (long double) 0.62137;
}

#endif //UTIL_HPP

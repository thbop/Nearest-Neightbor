#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double frac( double v ) {
    return v - floor(v);
}

double noise( int seed, double x ) {
    return frac( sin( x * 209.2342 ) * seed * 839.7301 );
}

struct vec3 {
    double x, y, z;
} typedef vec3;

vec3 v3subtract( vec3 a, vec3 b ) {
    return (vec3){ a.x-b.x, a.y-b.y, a.z-b.z };
}

vec3 v3noise( int seed, double x ) {
    return (vec3){
        noise( seed, x ),
        noise( seed+153, x+238 ),
        noise( seed+234, x+851 )
    };
}

double length( vec3 a ) {
    return sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
}

double v3distance( vec3 a, vec3 b ) {
    vec3 dis = v3subtract(b, a);
    return length(dis);
}

void v3print( vec3 v ) {
    printf( "vec3( %lf, %lf, %lf )\n", v.x, v.y, v.z );
}

struct v3buffer {
    vec3* data;
    int length;
} typedef v3buffer;

// Set noise_seed to zero if you want all zero values
v3buffer generate_buffer( int length, int noise_seed ) {
    v3buffer buffer;
    buffer.length = length;
    if ( noise_seed ) {
        buffer.data = (vec3*)malloc( length * sizeof(vec3) );
        for ( int i = 0; i < length; i++ )
            buffer.data[i] = v3noise( noise_seed, i );
    }
    else buffer.data = (vec3*)calloc( length, sizeof(vec3) );

    return buffer;
}

void print_buffer( v3buffer buffer ) {
    for ( int i = 0; i < buffer.length; i++ )
        v3print( buffer.data[i] );
}

void free_buffer( v3buffer buffer ) {
    free(buffer.data);
}

vec3 nearest_neighbor_linear( v3buffer buffer, vec3 v ) {
    vec3 nearest = buffer.data[0];
    double last_distance;
    for ( int i = 1; i < buffer.length; i++ ) {
        double dist = v3distance( v, buffer.data[i] );
        if ( dist < last_distance ) nearest = buffer.data[i];
    }
    return nearest;
}


int main() {
    v3buffer buffer = generate_buffer(100, 34);
    vec3 a = { 0.43, 0.64, 0.298 };

    vec3 nearest = nearest_neighbor_linear(buffer, a);
    v3print(nearest);

    free_buffer(buffer);
    return 0;
}
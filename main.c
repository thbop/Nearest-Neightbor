#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double frac( double v ) {
    return v - floor(v);
}

double noise( int seed, double x ) {
    return frac( sin( x * 209.2342 ) * seed * 839.7301 );
}

double step( double v, double size ) {
    return round( v / size ) * size;
}

struct vec3 {
    double x, y, z;
} typedef vec3;

vec3 v3subtract( vec3 a, vec3 b ) {
    return (vec3){ a.x-b.x, a.y-b.y, a.z-b.z };
}

unsigned char v3equals( vec3 a, vec3 b ) {
    return ( a.x == b.x && a.y == b.y && a.z == b.z );
}

vec3 v3noise( int seed, double x ) {
    return (vec3){
        noise( seed, x ),
        noise( seed+153, x+238 ),
        noise( seed+234, x+851 )
    };
}

vec3 v3step( vec3 v, double size ) {
    return (vec3){
        step( v.x, size ),
        step( v.y, size ),
        step( v.z, size )
    };
}

double v3length( vec3 a ) {
    return sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
}

double v3distance( vec3 a, vec3 b ) {
    vec3 dis = v3subtract(b, a);
    return v3length(dis);
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

void add_vec3( v3buffer* buffer, vec3 v ) {
    buffer->length++;
    buffer->data = realloc(buffer->data, buffer->length);

    buffer->data[buffer->length-1] = v;
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

// A buffer with a position
struct v3chunk {
    vec3 position;
    v3buffer buffer;
} typedef v3chunk;

void free_chunk( v3chunk chunk ) {
    free_buffer( chunk.buffer );
}

struct v3space {
    v3chunk* chunks;
    int length;
} typedef v3space;

// This might be expensive
void add_chunk( v3space* space, v3chunk chunk ) {
    space->length++;
    space->chunks = realloc(space->chunks, space->length);

    space->chunks[space->length-1] = chunk;

}

int in_existing_chunk( v3space space, vec3 v, double chunk_size ) {
    for ( int i = 0; i < space.length; i++ ) {
        if ( v3equals( space.chunks[i].position, v3step(v, chunk_size) ) ) return i;
    }
    return -1;
}

// length = amount of vectors, not amount of chunks
v3space generate_space( int length, int noise_seed, double chunk_size ) {
    v3buffer raw_buffer = generate_buffer( length, noise_seed );
    v3space space;
    space.length = 0;
    space.chunks = (v3chunk*)malloc(sizeof(v3chunk));
    unsigned char first_chunk = 1;

    for ( int i = 0; i < raw_buffer.length; i++ ) {
        int chunk_id = in_existing_chunk( space, raw_buffer.data[i], chunk_size );

        if ( chunk_id != -1 ) {
            add_vec3( &space.chunks[chunk_id].buffer, raw_buffer.data[i] );
        } else {
            v3chunk chunk;
            chunk.position = v3step( raw_buffer.data[i], chunk_size );
            chunk.buffer = generate_buffer(1, 0);
            
            chunk.buffer.data[0] = raw_buffer.data[i];

            if ( first_chunk ) { first_chunk = 0; space.chunks[0] = chunk; }
            else add_chunk( &space, chunk );
        }
            
    }

    free_buffer(raw_buffer);

    return space;
}

void print_space( v3space space ) {
    for ( int i = 0; i < space.length; i++ ) {
        printf("\nChunk id: ");
        v3print( space.chunks[i].position );
        print_buffer( space.chunks[i].buffer );
    }
}


void free_space( v3space space ) {
    for (int i = 0; i < space.length; i++) {
        free_chunk( space.chunks[i] );
    }
}


int main() {
    // v3buffer buffer = generate_buffer(100, 34);
    // vec3 a = { 0.43, 0.64, 0.298 };

    // vec3 nearest = nearest_neighbor_linear(buffer, a);
    // v3print(nearest);

    // free_buffer(buffer);

    v3space space = generate_space( 2, 365, 0.3 );

    print_space(space);

    free_space( space );

    return 0;
}
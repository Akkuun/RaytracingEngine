#define SPHERE 1
#define SQUARE 2
#define TRIANGLE 3
#define MESH 4
#define BVH 5

#define EPSILON 0.001f

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define BUFFER_IMAGE 0
#define BUFFER_ALBEDO 1
#define BUFFER_DEPTH 2
#define BUFFER_NORMAL 3

// Match CPU-side Vec3 with padding to align to 16 bytes (same as float4)
typedef struct {
    float x, y, z;
    float _padding;
} Vec3;

// GPU Camera structure matching CPU side exactly (64 bytes total)
typedef struct {
    Vec3 origin;      // Camera position (16 bytes)
    Vec3 target;      // What the camera is looking at (16 bytes)  
    Vec3 up;          // Up vector (16 bytes)
    float fov;        // Field of view in degrees (4 bytes)
	int nbBounces;    // Number of ray bounces (4 bytes)
	int raysPerPixel; //  Number of rays per pixel (4 bytes)
    int bufferType;
} GPUCamera;

// Helper function to convert Vec3 to float3
float3 vec3_to_float3(Vec3 v) {
    return (float3)(v.x, v.y, v.z);
}

struct Ray{
	float3 origin;
	float3 dir;
};

// Simple random number generator (PCG hash)
uint wang_hash(uint seed)
{
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	return seed;
}

// Generate random float in [0,1)
float random_float(uint* seed)
{
	*seed = wang_hash(*seed);
	return (float)(*seed) / 4294967296.0f;
}

// Generate random direction in hemisphere oriented around normal
// Uses cosine-weighted importance sampling
float3 random_hemisphere_direction(float3 normal, uint* seed)
{
	// Generate two random numbers
	float r1 = random_float(seed);
	float r2 = random_float(seed);
	
	// Cosine-weighted hemisphere sampling
	float phi = 2.0f * M_PI * r1;
	float cos_theta = sqrt(1.0f - r2);
	float sin_theta = sqrt(r2);
	
	// Create local coordinate system around normal
	float3 tangent;
	if (fabs(normal.x) > 0.1f) {
		tangent = normalize(cross((float3)(0.0f, 1.0f, 0.0f), normal));
	} else {
		tangent = normalize(cross((float3)(1.0f, 0.0f, 0.0f), normal));
	}
	float3 bitangent = cross(normal, tangent);
	
	// Transform from local to world space
	float3 direction = cos(phi) * sin_theta * tangent +
	                   sin(phi) * sin_theta * bitangent +
	                   cos_theta * normal;
	
	return normalize(direction);
}

struct Light {
	float3 pos;
	float3 color;
	float intensity;
};

// Match CPU-side GPUSphere exactly
typedef struct __attribute__((aligned(16))) {
    float radius;           // 4 bytes (offset 0)
    float _padding1[3];     // 12 bytes (offset 4)
    Vec3 pos;               // 16 bytes (offset 16)
	int materialIndex;      // 4 bytes (offset 32)
	float _padding2[3];     // 12 bytes (offset 36)
} GPUSphere;  // Total: 48 bytes

// Match CPU-side GPUSquare exactly
typedef struct __attribute__((aligned(16))) {
    Vec3 pos;               // 16 bytes (offset 0)
    Vec3 u_vec;             // 16 bytes (offset 16)
    Vec3 v_vec;             // 16 bytes (offset 32)
    Vec3 normal;            // 16 bytes (offset 48)
	int materialIndex;      // 4 bytes (offset 64)
	float _padding[3];      // 12 bytes (offset 68)
} GPUSquare;  // Total: 80 bytes

typedef struct __attribute__((aligned(16))) {
	Vec3 v0;                // 16 bytes (offset 0)
	Vec3 v1;                // 16 bytes (offset 16)
	Vec3 v2;                // 16 bytes (offset 32)
	int materialIndex;      // 4 bytes (offset 48)
	float _padding[3];      // 12 bytes (offset 52)
} GPUTriangle;  // Total: 64 bytes

// GPU-compatible BVH header structure (matches CPU-side GPUBVH)
typedef struct __attribute__((aligned(16))) {
    int node_offset;      // 4 bytes (offset 0)
    int triangle_offset;  // 4 bytes (offset 4)
    int node_count;       // 4 bytes (offset 8)
    int triangle_count;   // 4 bytes (offset 12)
    int material_index;   // 4 bytes (offset 16)
    float _padding[11];   // 44 bytes (offset 20)
} GPUBVH;  // Total: 64 bytes

// GPU-compatible AABB structure
typedef struct __attribute__((aligned(16))) {
    Vec3 minPoint;  // 16 bytes (offset 0)
    Vec3 maxPoint;  // 16 bytes (offset 16)
} AABBGPU;  // Total: 32 bytes

// GPU-compatible BVH Node structure
// If childIndex == -1, it's a leaf node and triangleStartIdx/triangleCount are valid
// Otherwise, childIndex points to the left child (right child is childIndex + 1)
typedef struct __attribute__((aligned(16))) {
    float boundingBoxMin[3]; // 12 bytes (offset 0)
	float _padding1;          // 4 bytes (offset 12)
	float boundingBoxMax[3]; // 12 bytes (offset 16)
	float _padding2;          // 4 bytes (offset 28)
	int startIndex;		// 4 bytes (offset 32) - start index in triangle array (for leaves)
	int triangleCount;        // 4 bytes (offset 36) - number of triangles (for leaves)
	float _padding3;          // 4 bytes (offset 40) - padding for 16-byte alignment
	float _padding4;          // 4 bytes (offset 44) - padding for 16-byte alignment
} GPUBVHNode;  // Total: 48 bytes

typedef struct __attribute__((aligned(16))) {
	Vec3 ambient;              // 16 bytes (offset 0)
	Vec3 diffuse;              // 16 bytes (offset 16)
	Vec3 specular;             // 16 bytes (offset 32)
	
	float shininess;           // 4 bytes (offset 48)
	float index_medium;        // 4 bytes (offset 52)
	float transparency;        // 4 bytes (offset 56)
	float texture_scale_x;     // 4 bytes (offset 60)
	
	float texture_scale_y;     // 4 bytes (offset 64)
	int emissive;              // 4 bytes (offset 68)
	float metalness;           // 4 bytes (offset 72)
	float _padding1;           // 4 bytes (offset 76) - CRITICAL for alignment!
	
	Vec3 light_color;          // 16 bytes (offset 80)
	
	float light_intensity;     // 4 bytes (offset 96)
	int has_texture;           // 4 bytes (offset 100)
	int texture_width;         // 4 bytes (offset 104)
	int texture_height;        // 4 bytes (offset 108)

	int texture_offset;        // 4 bytes (offset 112)
	int has_normal_map;        // 4 bytes (offset 116)
	int normal_map_width;      // 4 bytes (offset 120)
	int normal_map_height;     // 4 bytes (offset 124)

	int normal_map_offset;     // 4 bytes (offset 128)
	int has_metal_map;         // 4 bytes (offset 132)
	int metal_map_width;       // 4 bytes (offset 136)
	int metal_map_height;      // 4 bytes (offset 140)

	int metal_map_offset;      // 4 bytes (offset 144)
	int has_emissive_map;      // 4 bytes (offset 148)
	int emissive_map_width;    // 4 bytes (offset 152)
	int emissive_map_height;   // 4 bytes (offset 156)

	int emissive_map_offset;   // 4 bytes (offset 160)
	int material_id;           // 4 bytes (offset 164)
	int _padding2[2];          // 8 bytes (offset 168) - padding for 16-byte alignment
} GPUMaterial;  // Total: 176 bytes

struct Intersection {
	float t;
	float3 hitpoint;
	float3 normal;
	float2 uv;
	int hitShapeIndex;
};

// Match CPU-side GPUShape exactly
typedef struct __attribute__((aligned(16))) {
    int type;           // form type
    float _padding[3];  // Padding after int to align union to 16 bytes
    union {
        GPUSphere sphere;
        GPUSquare square;
		GPUTriangle triangle;
		GPUBVH bvh;
    } data;
	// int materialID;
} GPUShape;

// Sample texture at UV coordinates
float3 sample_texture(__global const unsigned char* textureData, int offset, int width, int height, float2 uv)
{
	if (offset < 0 || width <= 0 || height <= 0) {
		return (float3)(1.0f, 1.0f, 1.0f); // White default if no texture
	}
	
	// Wrap UV coordinates to [0,1]
	uv.x = uv.x - floor(uv.x);
	uv.y = uv.y - floor(uv.y);
	
	// Convert UV to pixel coordinates
	int x = (int)(uv.x * width) % width;
	int y = (int)(uv.y * height) % height;
	
	// Calculate pixel index in texture buffer (RGB = 3 bytes per pixel)
	int pixelIndex = offset + (y * width + x) * 3;
	
	// Read RGB values and normalize to [0,1]
	float r = (float)textureData[pixelIndex + 0] / 255.0f;
	float g = (float)textureData[pixelIndex + 1] / 255.0f;
	float b = (float)textureData[pixelIndex + 2] / 255.0f;
	
	return (float3)(r, g, b);
}

// Sample normal map at UV coordinates
float3 sample_normal_map(__global const unsigned char* textureData, int offset, int width, int height, float2 uv)
{
	if (offset < 0 || width <= 0 || height <= 0) {
		return (float3)(0.0f, 0.0f, 1.0f); // Default normal pointing up
	}
	
	// Wrap UV coordinates to [0,1]
	uv.x = uv.x - floor(uv.x);
	uv.y = uv.y - floor(uv.y);
	
	// Convert UV to pixel coordinates
	int x = (int)(uv.x * width) % width;
	int y = (int)(uv.y * height) % height;
	
	// Calculate pixel index in texture buffer (RGB = 3 bytes per pixel)
	int pixelIndex = offset + (y * width + x) * 3;
	
	// Read RGB values and normalize to [0,1], then remap to [-1,1]
	float r = ((float)textureData[pixelIndex + 0] / 255.0f) * 2.0f - 1.0f;
	float g = ((float)textureData[pixelIndex + 1] / 255.0f) * 2.0f - 1.0f;
	float b = ((float)textureData[pixelIndex + 2] / 255.0f) * 2.0f - 1.0f;
	
	return normalize((float3)(r, g, b));
}

// Sample metal map at UV coordinates (returns metalness value [0,1])
float sample_metal_map(__global const unsigned char* textureData, int offset, int width, int height, float2 uv)
{
	if (offset < 0 || width <= 0 || height <= 0) {
		return 0.0f; // Default metalness if no map
	}
	
	// Wrap UV coordinates to [0,1]
	uv.x = uv.x - floor(uv.x);
	uv.y = uv.y - floor(uv.y);
	
	// Convert UV to pixel coordinates
	int x = (int)(uv.x * width) % width;
	int y = (int)(uv.y * height) % height;
	
	// Calculate pixel index in texture buffer (RGB = 3 bytes per pixel)
	int pixelIndex = offset + (y * width + x) * 3;
	
	// Read red channel and normalize to [0,1] (metalness is typically stored in red channel)
	float metalness = (float)textureData[pixelIndex + 0] / 255.0f;
	
	return metalness;
}

// Compute tangent space basis (TBN matrix) for normal mapping
void compute_tangent_space(__global const GPUShape* shape, struct Intersection inter, float3* tangent, float3* bitangent, float3* normal)
{
	*normal = inter.normal;
	
	if (shape->type == SPHERE) {
		// For spheres, compute tangents from spherical UV mapping
		float theta = (inter.uv.x - 0.5f) * 2.0f * M_PI;
		float phi = (0.5f - inter.uv.y) * M_PI;
		
		// Tangent in theta direction
		*tangent = (float3)(-sin(theta), 0.0f, cos(theta));
		
		// Bitangent in phi direction  
		*bitangent = (float3)(cos(theta) * cos(phi), -sin(phi), sin(theta) * cos(phi));
		
	} else if (shape->type == SQUARE) {
		// For squares, use the u_vec and v_vec as tangents
		float3 u_vec = vec3_to_float3(shape->data.square.u_vec);
		float3 v_vec = vec3_to_float3(shape->data.square.v_vec);
		
		*tangent = normalize(u_vec);
		*bitangent = normalize(v_vec);
		
	} else if (shape->type == TRIANGLE) {
		// For triangles, we need to compute tangents from UV coordinates
		// This is a simplified version - in practice you'd want proper tangent calculation
		// For now, use a simple approximation
		float3 edge1 = vec3_to_float3(shape->data.triangle.v1) - vec3_to_float3(shape->data.triangle.v0);
		float3 edge2 = vec3_to_float3(shape->data.triangle.v2) - vec3_to_float3(shape->data.triangle.v0);
		
		float2 deltaUV1 = (float2)(inter.uv.x, inter.uv.y) - (float2)(0.0f, 0.0f); // Simplified
		float2 deltaUV2 = (float2)(0.0f, 0.0f) - (float2)(0.0f, 0.0f); // This is not correct, need proper UVs
		
		// Fallback: use arbitrary tangents perpendicular to normal
		if (fabs(normal->x) > 0.1f) {
			*tangent = normalize(cross((float3)(0.0f, 1.0f, 0.0f), *normal));
		} else {
			*tangent = normalize(cross((float3)(1.0f, 0.0f, 0.0f), *normal));
		}
		*bitangent = cross(*normal, *tangent);
	}
}

// Get the final normal including normal map perturbation
float3 get_perturbed_normal(__global const GPUShape* shape, struct Intersection inter, __global const GPUMaterial* material, __global const unsigned char* textureData)
{
	float3 geometric_normal = inter.normal;
	
	if (material == NULL || !material->has_normal_map) {
		return geometric_normal;
	}
	
	// Sample normal from normal map (in tangent space)
	float3 tangent_normal = sample_normal_map(textureData, material->normal_map_offset, 
	                                         material->normal_map_width, material->normal_map_height, inter.uv);
	
	// Compute tangent space basis
	float3 tangent, bitangent, normal;
	compute_tangent_space(shape, inter, &tangent, &bitangent, &normal);
	
	// Transform tangent space normal to world space
	float3 world_normal = tangent * tangent_normal.x + 
	                     bitangent * tangent_normal.y + 
	                     normal * tangent_normal.z;
	
	return normalize(world_normal);
}

// Get material by materialIndex with O(1) direct access
__global const GPUMaterial* get_material_by_index(int materialIndex, __global const GPUMaterial* materials, int numMaterials)
{
	if (materialIndex < 0 || materialIndex >= numMaterials) return NULL;
	
	if (materials[materialIndex].material_id == materialIndex) {
		return &materials[materialIndex];
	}
	
	return NULL;
}

float3 checkerboard_texture(float2 uv, float3 color1, float3 color2, float scale)
{
	int checkX = (int)(floor(uv.x * scale));
	int checkY = (int)(floor(uv.y * scale));
	
	if ((checkX + checkY) % 2 == 0) {
		return color1;
	} else {
		return color2;
	}
}

int get_shape_material_index(__global const GPUShape* shape, __global const GPUMaterial* materials, int numMaterials)
{
	int materialIndex = -1;
	
	// Get materialIndex from shape
	if (shape->type == SPHERE) {
		materialIndex = shape->data.sphere.materialIndex;
	} else if (shape->type == SQUARE) {
		materialIndex = shape->data.square.materialIndex;
	} else if (shape->type == TRIANGLE) {
		materialIndex = shape->data.triangle.materialIndex;
	} else if (shape->type == MESH || shape->type == BVH) {
		materialIndex = shape->data.bvh.material_index;
	}
	
	// Get material - if not found, return NULL
	return materialIndex;
}

float3 reflect(float3 incident, float3 normal)
{
	return incident - 2.0f * dot(incident, normal) * normal;
}

float3 refract_direction(float3 incident, float3 normal, float eta)
{
    float cosI = -dot(incident, normal);
    float sinT2 = eta * eta * (1.0f - cosI * cosI);
    if (sinT2 > 1.0f) return (float3)(0.0f, 0.0f, 0.0f);
    float cosT = sqrt(1.0f - sinT2);
    return eta * incident + (eta * cosI - cosT) * normal;
}

float fresnel_schlick(float cosI, float n1, float n2)
{
    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    float x = 1.0f - cosI;
    return r0 + (1.0f - r0) * x*x*x*x*x;
}

float3 get_reflected_ray(float3 incident, struct Intersection inter, __global const GPUShape* shape, __global const GPUMaterial* material, __global const unsigned char* textureData, uint* seed)
{
	if (material == NULL) {
		// Default to diffuse reflection
		return random_hemisphere_direction(inter.normal, seed);
	}
	
	// Get the perturbed normal (includes normal map if available)
	float3 normal = get_perturbed_normal(shape, inter, material, textureData);
	
	// Get metalness value - use metal map if available, otherwise use material metalness
	float metalness = material->metalness;
	if (material->has_metal_map) {
		metalness = sample_metal_map(textureData, material->metal_map_offset, 
		                            material->metal_map_width, material->metal_map_height, inter.uv);
	}
	
	// Continuous metalness: blend between diffuse and specular reflection
	float3 diffuse = random_hemisphere_direction(normal, seed);
	float3 reflected = reflect(incident, normal);
	
	// Add roughness based on metalness (higher metalness = lower roughness)
	float roughness = 1.0f - metalness;
	float3 randomDir = random_hemisphere_direction(normal, seed);
	float3 roughReflected = normalize(mix(reflected, randomDir, roughness));
	
	// Blend between diffuse and rough specular based on metalness
	return normalize(mix(diffuse, roughReflected, metalness));
} 

float3 get_shape_color(__global const GPUShape* shape, __global const GPUMaterial* materials, int numMaterials, 
                       __global const unsigned char* textureData, float2 uv)
{
	int materialIndex = get_shape_material_index(shape, materials, numMaterials);
	
	// Get material - if not found, use white as default
	__global const GPUMaterial* material = get_material_by_index(materialIndex, materials, numMaterials);
	
	if (material == NULL) {
		// No material found, return white
		return checkerboard_texture(uv, (float3)(0.502f, 0.502f, 0.502f), (float3)(0.627f, 0.627f, 0.643f), 10.0f);
	}
	
	// Check if material has texture
	if (material->has_texture) {
		return sample_texture(textureData, material->texture_offset, 
		                     material->texture_width, material->texture_height, uv);
	}
	
	// No texture, use material diffuse color
	return vec3_to_float3(material->diffuse);
}

// Optimized intersection functions with inline and restrict
inline __attribute__((always_inline)) struct Intersection intersect_sphere(__global const GPUSphere* restrict sphere, const struct Ray* restrict ray, float* restrict t)
{
	float3 sphere_pos = vec3_to_float3(sphere->pos);
	float3 rayToCenter = sphere_pos - ray->origin;

	/* calculate coefficients a, b, c from quadratic equation */
	/* float a = dot(ray->dir, ray->dir); // ray direction is normalised, dotproduct simplifies to 1 */ 
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius * sphere->radius;
	float disc = b * b - c; /* discriminant of quadratic formula */

	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */

	if (disc < 0.0f) return result; /* no intersection */

	/* solve for t (distance to hitpoint along ray) */
	float sqrt_disc = sqrt(disc);
	float t1 = b - sqrt_disc; /* near intersection */
	float t2 = b + sqrt_disc; /* far intersection */

	/* choose the closest positive t with epsilon threshold */
	*t = -1.0f;
	if (t1 > EPSILON) {
		*t = t1;
	} else if (t2 > EPSILON) {
		*t = t2; /* ray starts inside sphere, use exit point */
	} else {
		return result; /* both intersections behind ray or too close */
	}

	result.t = *t;
	result.hitpoint = ray->origin + ray->dir * (*t);
	
	/* calculate normal - always point outward from sphere center */
	result.normal = normalize(result.hitpoint - sphere_pos);
	
	/* backface culling: if ray and normal point in same direction, reject */
	if (dot(ray->dir, result.normal) > 0.0f && *t == t1) {
		/* hitting sphere from behind (outside->inside), reject this intersection */
		result.t = -1.0f;
		return result;
	}
	
	/* if we're using the far intersection (t2), we're inside the sphere */
	/* so flip the normal to point inward for proper shading */
	if (*t == t2) {
		result.normal = -result.normal;
	}
	
	result.uv = (float2)(0.5f + (atan2(result.normal.z, result.normal.x) / (2.0f * M_PI)),
	                      0.5f - (asin(result.normal.y) / M_PI)); // Spherical UV mapping
	return result;
}

inline __attribute__((always_inline)) struct Intersection intersect_square(__global const GPUSquare* restrict square, const struct Ray* restrict ray, float* restrict t)
{
    /* calculate intersection of ray with plane of square */

    struct Intersection result;
    result.t = -1.0f; /* default to no intersection */

    float3 square_normal = vec3_to_float3(square->normal);
    float3 square_pos = vec3_to_float3(square->pos);
    float3 square_u_vec = vec3_to_float3(square->u_vec);
    float3 square_v_vec = vec3_to_float3(square->v_vec);

    float denom = dot(square_normal, ray->dir);
    if (fabs(denom) < EPSILON) return result; /* ray is parallel to square plane */

    /* backface culling: if ray and normal point in same direction, reject */
    if (denom > 0.0f) return result; /* hitting square from behind */

    float3 diff = square_pos - ray->origin;
    *t = dot(diff, square_normal) / denom;

    if (*t < EPSILON) return result; /* square is behind ray or too close */

    /* calculate hitpoint */
    float3 hitpoint = ray->origin + ray->dir * (*t);
    result.t = *t;
    result.hitpoint = hitpoint;
    result.normal = square_normal;

    /* check if hitpoint is inside square bounds */
    float3 local_pos = hitpoint - square_pos;

    // Get the actual lengths of the vectors
    float u_length = length(square_u_vec);
    float v_length = length(square_v_vec);
    
    // Normalize the vectors for projection
    float3 u_normalized = square_u_vec / u_length;
    float3 v_normalized = square_v_vec / v_length;

    float u_dist = dot(local_pos, u_normalized);
    if (u_dist < -u_length * 0.5f || u_dist > u_length * 0.5f) {
        result.t = -1.0f;
        return result;
    }

    float v_dist = dot(local_pos, v_normalized);
    if (v_dist < -v_length * 0.5f || v_dist > v_length * 0.5f) {
        result.t = -1.0f;
        return result;
    }

    result.uv = (float2)((u_dist / u_length) + 0.5f, 1.0f - ((v_dist / v_length) + 0.5f)); // UV coordinates in [0,1] range

    return result;
}

inline __attribute__((always_inline)) struct Intersection intersect_triangle(__global const GPUTriangle* restrict triangle, const struct Ray* restrict ray, float* restrict t)
{
	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */

	float3 v0 = vec3_to_float3(triangle->v0);
	float3 v1 = vec3_to_float3(triangle->v1);
	float3 v2 = vec3_to_float3(triangle->v2);

	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	 float3 h = cross(ray->dir, edge2);
	 float a = dot(edge1, h);

	 /* Backface culling: reject triangles whose normal faces away from the ray
		 (also rejects near-parallel cases when a is very small) */
	 if (a < EPSILON) return result;

	float f = 1.0f / a;
	float3 s = ray->origin - v0;
	float u = f * dot(s, h);

	if (u < 0.0f || u > 1.0f) return result;

	float3 q = cross(s, edge1);
	float v = f * dot(ray->dir, q);

	if (v < 0.0f || u + v > 1.0f) return result;

	*t = f * dot(edge2, q);

	if (*t < EPSILON) return result; /* triangle is behind ray or too close */

	result.t = *t;
	result.hitpoint = ray->origin + ray->dir * (*t);
	result.normal = normalize(cross(edge1, edge2));
	result.uv = (float2)(u, v); // Barycentric coordinates as UV

	return result;
}

inline __attribute__((always_inline)) float intersect_aabb(__global const GPUBVHNode* restrict node, const struct Ray* restrict ray)
{
	float3 invDir = 1.0f / ray->dir;
	Vec3 minVec = {node->boundingBoxMin[0], node->boundingBoxMin[1], node->boundingBoxMin[2], 0.0f};
	Vec3 maxVec = {node->boundingBoxMax[0], node->boundingBoxMax[1], node->boundingBoxMax[2], 0.0f};
	float3 t0s = (vec3_to_float3(minVec) - ray->origin) * invDir;
	float3 t1s = (vec3_to_float3(maxVec) - ray->origin) * invDir;

	float3 tsmaller = fmin(t0s, t1s);
	float3 tbigger = fmax(t0s, t1s);

	float tmin = fmax(fmax(tsmaller.x, tsmaller.y), fmax(tsmaller.z, EPSILON));
	float tmax = fmin(fmin(tbigger.x, tbigger.y), tbigger.z);

	if (tmax >= tmin) {
		return tmin; /* intersection occurs at tmin */
	} else {
		return 1e20; /* no intersection */
	}
}

inline __attribute__((always_inline)) struct Intersection intersect_bvh(
	__global const GPUBVH* restrict bvh,
	__global const GPUBVHNode* restrict nodes,
	__global const GPUTriangle* restrict triangles,
	const struct Ray* restrict ray)
{

	int root = bvh->node_offset;

	// Stack for iterative BVH traversal
	int stack[64];
	int stackPtr = 0;
	int stackSize = 1;
	stack[stackPtr++] = root;

	float minDst = 1e20;
	int hitTriangleIndex = -1;
	struct Intersection closestIntersection;
	closestIntersection.t = -1.0f;

	while (stackSize > 0) {
		GPUBVHNode node = nodes[stack[--stackPtr]];
		stackSize--;

		if (node.triangleCount > 0) {
			for (int i = 0; i < node.triangleCount; i++) {
				int triIndex = node.startIndex + i;
				float t_temp = 1e20;
				struct Intersection intersection = intersect_triangle(&triangles[triIndex], ray, &t_temp);

				if (intersection.t > EPSILON && intersection.t < minDst) {
					minDst = intersection.t;
					hitTriangleIndex = triIndex;
					closestIntersection = intersection;
				}
			}
		}
		else
		{
			int leftChildIndex = node.startIndex;
			int rightChildIndex = node.startIndex + 1;

			float dstA = intersect_aabb(&nodes[leftChildIndex], ray);
			float dstB = intersect_aabb(&nodes[rightChildIndex], ray);

			if (dstA > dstB)
			{
				if (dstA < minDst)
				{
					stack[stackPtr++] = leftChildIndex;
					stackSize++;
				}
				if (dstB < minDst)
				{
					stack[stackPtr++] = rightChildIndex;
					stackSize++;
				}
			}
			else
			{
				if (dstB < minDst)
				{
					stack[stackPtr++] = rightChildIndex;
					stackSize++;
				}
				if (dstA < minDst)
				{
					stack[stackPtr++] = leftChildIndex;
					stackSize++;
				}
			}
		}
	}

	return closestIntersection;
}

inline __attribute__((always_inline)) struct Intersection intersect_shape(
	__global const GPUShape* restrict shape,
	const struct Ray* restrict ray,
	float* restrict t,
	__global const GPUBVHNode* restrict nodes,
	__global const GPUTriangle* restrict triangles)
{
	if (shape->type == SPHERE) {
		return intersect_sphere(&shape->data.sphere, ray, t);
	} else if (shape->type == SQUARE) {
		return intersect_square(&shape->data.square, ray, t);
	} else if (shape->type == TRIANGLE) {
		return intersect_triangle(&shape->data.triangle, ray, t);
	} else if (shape->type == MESH || shape->type == BVH) {
		return intersect_bvh(&shape->data.bvh, nodes, triangles, ray);
	}
	struct Intersection result;
	result.t = -1.0f; /* default to no intersection */
	return result;
}

inline __attribute__((always_inline)) struct Intersection compute_intersection(
	__global const GPUShape* restrict shapes, 
	int numShapes, 
	const struct Ray* restrict ray,
	__global const GPUBVHNode* restrict nodes,
	__global const GPUTriangle* restrict triangles)
{
	float t = 1e20;
	int hitShapeIndex = -1;
	struct Intersection finalIntersection;
	finalIntersection.t = -1.0f; /* default to no intersection */

	for (int i = 0; i < numShapes; i++){
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&shapes[i], ray, &t_temp, nodes, triangles);

		if (intersection.t > EPSILON && intersection.t < t){
			t = intersection.t;
			hitShapeIndex = i;
			finalIntersection = intersection;
			finalIntersection.hitShapeIndex = hitShapeIndex;
		}
	}

	return finalIntersection;
}

inline __attribute__((always_inline)) bool compute_shadow(
	__global const GPUShape* restrict shapes, 
	int numShapes, 
	const struct Ray* restrict shadowRay, 
	float maxDistance,
	__global const GPUBVHNode* restrict nodes,
	__global const GPUTriangle* restrict triangles)
{
	for (int i = 0; i < numShapes; i++){
		float t_temp = 1e20;
		struct Intersection intersection;
		intersection = intersect_shape(&shapes[i], shadowRay, &t_temp, nodes, triangles);

		if (intersection.t > EPSILON && intersection.t < maxDistance){
			return true; /* in shadow */
		}
	}

	return false; /* not in shadow */
}

// Iterative version to avoid recursion issues with Rusticl driver
// Uses hemisphere sampling for diffuse materials
float3 raytrace_iterative(
	const struct Ray* initialRay, 
	__global const GPUShape* shapes, 
	int numShapes, 
	const struct Light* lights, 
	int numLights, 
	int maxBounces, 
	uint* seed, 
	__global const GPUMaterial* materials, 
	int numMaterials, 
	__global const unsigned char* textureData,
	__global const GPUBVHNode* restrict nodes,
	__global const GPUTriangle* restrict triangles)
{
	float3 accumulatedColor = (float3)(0.0f, 0.0f, 0.0f);
	float3 throughput = (float3)(1.0f, 1.0f, 1.0f); // Track how much light can pass through
	float currentIOR = 1.0f;
	struct Ray currentRay = *initialRay;
	
	for (int bounce = 0; bounce < maxBounces; bounce++) {
		struct Intersection intersection = compute_intersection(shapes, numShapes, &currentRay, nodes, triangles);
		
		if (intersection.t < EPSILON) {
			// No intersection, could add sky color here
			break;
		}

		float3 diffuse = get_shape_color(&shapes[intersection.hitShapeIndex], materials, numMaterials, textureData, intersection.uv);
		
		// Direct lighting contribution
		float3 directLight = (float3)(0.0f, 0.0f, 0.0f);
		
		// Ambient term
		directLight += diffuse * 0.25f;

		// Direct lighting from light sources
		for (int i = 0; i < numLights; i++){
			float3 lightDir = normalize(lights[i].pos - intersection.hitpoint);
			float dotLN = dot(lightDir, intersection.normal);

			if (dotLN > EPSILON) {
				struct Ray shadowRay;
				shadowRay.origin = intersection.hitpoint + intersection.normal * EPSILON * 10.0f;
				shadowRay.dir = lightDir;
				float lightDistance = length(lights[i].pos - intersection.hitpoint);
				
				if (!compute_shadow(shapes, numShapes, &shadowRay, lightDistance - EPSILON, nodes, triangles)) {
					// Not in shadow - add full lighting
					directLight += diffuse * lights[i].color * lights[i].intensity * dotLN;
				} else {
					// In shadow - add reduced lighting
					directLight += diffuse * lights[i].color * lights[i].intensity * dotLN * 0.2f;
				}
			}
		}

		// Add direct lighting modulated by throughput
		accumulatedColor += throughput * directLight;

		// Russian roulette termination for efficiency
		float maxThroughput = fmax(fmax(throughput.x, throughput.y), throughput.z);
		if (maxThroughput < 0.01f) break; // Stop if contribution is too small

		// Prepare next ray
		if (bounce < maxBounces - 1) {
			__global const GPUMaterial* material = get_material_by_index(get_shape_material_index(&shapes[intersection.hitShapeIndex], materials, numMaterials), materials, numMaterials);
			if (material && material->transparency > 0.0f) {
				// Dielectric material - refraction/reflection
				float3 normal = intersection.normal;
				float n1 = currentIOR;
				float n2 = material->index_medium;
				bool entering = dot(currentRay.dir, normal) < 0;
				if (!entering) {
					normal = -normal;
					float temp = n1;
					n1 = n2;
					n2 = temp;
				}
				float eta = n1 / n2;
				float cosI = -dot(currentRay.dir, normal);
				cosI = clamp(cosI, 0.0f, 1.0f);
				float R = fresnel_schlick(cosI, n1, n2);
				float rand = random_float(seed);
				float3 newDir;
				if (rand < R) {
					// Reflect
					newDir = reflect(currentRay.dir, normal);
				} else {
					// Refract
					newDir = refract_direction(currentRay.dir, normal, eta);
					if (length(newDir) < 0.1f) {
						// Total internal reflection
						newDir = reflect(currentRay.dir, normal);
					} else {
						currentIOR = n2;
					}
				}
				currentRay.dir = normalize(newDir);
				throughput *= (float3)(1.0f, 1.0f, 1.0f);
			} else {
				// Opaque material - reflection
				throughput *= diffuse;
				float3 newDir = get_reflected_ray(currentRay.dir, intersection, &shapes[intersection.hitShapeIndex], material, textureData, seed);
				currentRay.dir = newDir;
			}
			currentRay.origin = intersection.hitpoint + currentRay.dir * EPSILON * 10.0f;
		}
	}

	return accumulatedColor;
}

// Original function that worked with GPUCamera (for reference)
struct Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height, __global const GPUCamera* camera){

	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	
	/* Convert FOV from degrees to radians */
	float fov_radians = camera->fov * M_PI / 180.0f;
	float tan_half_fov = tan(fov_radians * 0.5f);
	
	/* Calculate camera coordinate system */
	float3 camera_origin = vec3_to_float3(camera->origin);
	float3 camera_target = vec3_to_float3(camera->target);
	float3 camera_up = vec3_to_float3(camera->up);
	
	float3 forward = normalize(camera_target - camera_origin);
	float3 right = normalize(cross(forward, camera_up));
	float3 up = cross(right, forward);
	
	/* Calculate pixel position in camera space */
	float px = (fx - 0.5f) * 2.0f * tan_half_fov * aspect_ratio;
	float py = -(fy - 0.5f) * 2.0f * tan_half_fov;  // Invert Y for image coordinates
	
	/* Ray direction in world space */
	float3 ray_dir = normalize(forward + px * right + py * up);

	/* create camera ray*/
	struct Ray ray;
	ray.origin = camera_origin;
	ray.dir = ray_dir;

	return ray;
}

// __global output -> [R,G,B,R,G,B,...]
// __global accumBuffer -> accumulates samples over frames [R,G,B,R,G,B,...]
// frameCount -> number of frames accumulated so far (resets when camera/scene changes)
__kernel void render_kernel(__global float* output, __global float* accumBuffer, int width, int height, int frameCount, 
                           __global GPUShape* shapes, int numShapes,
                           __global GPUCamera* camera, __global GPUMaterial* materials, int numMaterials,
                           __global unsigned char* textureData,
						   int numBVHNodes, __global const GPUBVHNode* bvhNodes,
						   int numBVHTriangles, __global const GPUTriangle* bvhTriangles)
{
	const int work_item_id = get_global_id(0);		/* id of current pixel that we are working with */
	int x_coord = work_item_id % width;					/* x-coordinate of the pixel */
	int y_coord = work_item_id / width;					/* y-coordinate of the pixel */

    if (work_item_id >= width * height) return;
    
	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/*create a camera ray */
	struct Ray camray = createCamRay(x_coord, y_coord, width, height, camera);

	struct Light lights[1];
	lights[0].pos = (float3)(0.0f, 0.2f, 0.0f);
	lights[0].color = (float3)(1.0f, 1.0f, 1.0f);
	lights[0].intensity = 1.0f;

	int numLights = sizeof(lights) / sizeof(lights[0]);

	int maxbounce = camera->nbBounces;
	
	// Initialize random seed based on pixel position AND frame count for temporal variation
	uint seed = (x_coord * 1973 + y_coord * 9277 + frameCount * 26699) | 1;
	
	float3 outputPixelColor = (float3)(0.0f, 0.0f, 0.0f);
	if (camera->bufferType == BUFFER_IMAGE) {
		outputPixelColor = raytrace_iterative(&camray, shapes, numShapes, lights, numLights, maxbounce, &seed, materials, numMaterials, textureData, bvhNodes, bvhTriangles);

		/* If no intersection found, return background colour */
		if (outputPixelColor.x == 0.0f && outputPixelColor.y == 0.0f && outputPixelColor.z == 0.0f) {
			outputPixelColor = (float3)(fy * 0.7f, fy * 0.3f, 0.3f);
		}
	} else if (camera->bufferType == BUFFER_ALBEDO) {
		struct Intersection intersection = compute_intersection(shapes, numShapes, &camray, bvhNodes, bvhTriangles);
		if (intersection.t > EPSILON) {
			outputPixelColor = get_shape_color(&shapes[intersection.hitShapeIndex], materials, numMaterials, textureData, intersection.uv);
		} else {
			outputPixelColor = (float3)(0.0f, 0.0f, 0.0f);
		}
	} else if (camera->bufferType == BUFFER_NORMAL) {
		struct Intersection intersection = compute_intersection(shapes, numShapes, &camray, bvhNodes, bvhTriangles);
		if (intersection.t > EPSILON) {
			float3 normal = get_perturbed_normal(&shapes[intersection.hitShapeIndex], intersection, get_material_by_index(get_shape_material_index(&shapes[intersection.hitShapeIndex], materials, numMaterials), materials, numMaterials), textureData);
			outputPixelColor = normal * 0.5f + 0.5f; // Map from [-1,1] to [0,1]
		} else {
			outputPixelColor = (float3)(0.0f, 0.0f, 0.0f);
		}
	} else if (camera->bufferType == BUFFER_DEPTH) {
		struct Intersection intersection = compute_intersection(shapes, numShapes, &camray, bvhNodes, bvhTriangles);
		if (intersection.t > EPSILON) {
			// Map depth to [0,1] range for visualization
			float depth = intersection.t;
			float maxDepth = 4.0f; // Arbitrary max depth for normalization
			float depthValue = clamp(depth / maxDepth, 0.0f, 1.0f);
			outputPixelColor = (float3)(depthValue, depthValue, depthValue);
		} else {
			outputPixelColor = (float3)(0.0f, 0.0f, 0.0f);
		}
	}
	
	// index *3 for RGB
	int base_idx = work_item_id * 3;
	
	// Temporal accumulation: blend new sample with accumulated samples
	float3 accumulatedColor;
	if (frameCount == 0) {
		// First frame: just use current sample
		accumulatedColor = outputPixelColor;
	} else {
		// Progressive accumulation using running average
		float3 previousAccum = (float3)(accumBuffer[base_idx], 
		                                 accumBuffer[base_idx + 1], 
		                                 accumBuffer[base_idx + 2]);
		
		// Running average: new_avg = (old_avg * n + new_sample) / (n + 1)
		float t = (float)frameCount / (float)(frameCount + 1);
		accumulatedColor = previousAccum * t + outputPixelColor * (1.0f - t);
	}
	
	// Store accumulated color (linear space)
	accumBuffer[base_idx] = accumulatedColor.x;
	accumBuffer[base_idx + 1] = accumulatedColor.y;
	accumBuffer[base_idx + 2] = accumulatedColor.z;
	
	// Apply post-processing for display
	float3 displayColor = clamp(accumulatedColor, 0.0f, 1.0f);
	// Apply gamma correction (gamma = 2.2)
	//displayColor = pow(displayColor, (float3)(1.0f / 2.2f));
	
	output[base_idx] = displayColor.x;     // R
	output[base_idx + 1] = displayColor.y; // G
	output[base_idx + 2] = displayColor.z; // B
}


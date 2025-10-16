struct Ray{
	float3 origin;
	float3 dir;
};

struct Sphere{
	float radius;
	float3 pos;
	float3 emi;
	float3 color;
};

bool intersect_sphere(const struct Sphere* sphere, const struct Ray* ray, float* t)
{
	float3 rayToCenter = sphere->pos - ray->origin;

	/* calculate coefficients a, b, c from quadratic equation */

	/* float a = dot(ray->dir, ray->dir); // ray direction is normalised, dotproduct simplifies to 1 */ 
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c; /* discriminant of quadratic formula */

	/* solve for t (distance to hitpoint along ray) */

	if (disc < 0.0f) return false;
	else *t = b - sqrt(disc);

	if (*t < 0.0f){
		*t = b + sqrt(disc);
		if (*t < 0.0f) return false; 
	}

	else return true;
}

struct Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height){

	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	float fx2 = (fx - 0.5f) * aspect_ratio;
	float fy2 = fy - 0.5f;

	/* determine position of pixel on screen */
	float3 pixel_pos = (float3)(fx2, -fy2, 0.0f);

	/* create camera ray*/
	struct Ray ray;
	ray.origin = (float3)(0.0f, 0.0f, 40.0f); /* fixed camera position */
	ray.dir = normalize(pixel_pos - ray.origin); /* ray direction is vector from camera to pixel */

	return ray;
}
// __global output -> [R,G,B,R,G,B,...]
__kernel void render_kernel(__global float* output, int width, int height)
{
	const int work_item_id = get_global_id(0);		/* id of current pixel that we are working with */
	int x_coord = work_item_id % width;					/* x-coordinate of the pixel */
	int y_coord = work_item_id / width;					/* y-coordinate of the pixel */

    if (work_item_id >= width * height) return;
    
	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/*create a camera ray */
	struct Ray camray = createCamRay(x_coord, y_coord, width, height);

	/* create and initialise a sphere */
	struct Sphere sphere1;
	sphere1.radius = 0.4f;
	sphere1.pos = (float3)(0.0f, 0.0f, 3.0f);
	sphere1.color = (float3)(0.9f, 0.3f, 0.0f);

	/* intersect ray with sphere */
	float t = 1e20;
	intersect_sphere(&sphere1, &camray, &t);

	/* if ray misses sphere, return background colour 
	background colour is a blue-ish gradient dependent on image height */
	float3 color,normal,outputPixelColor;
	
    if (t > 1e19) { 
        outputPixelColor = (float3)(fy * 0.1f, fy * 0.3f, 0.3f);
    } else {
        float3 hitpoint = camray.origin + camray.dir * t;
        normal = normalize(hitpoint - sphere1.pos);
        float cosine_factor = dot(normal, camray.dir) * -1.0f;
        color = sphere1.color * cosine_factor;
		outputPixelColor = normal * 0.5f + (float3)(0.5f, 0.5f, 0.5f);
    }
    
    // index *3 for RGB
    int base_idx = work_item_id * 3;
	
    output[base_idx] = outputPixelColor.x;     // R
    output[base_idx + 1] = outputPixelColor.y; // G
    output[base_idx + 2] = outputPixelColor.z; // B

}
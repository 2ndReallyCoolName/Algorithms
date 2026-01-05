#pragma once
#include <vector>

struct vec3 {
	float x;
	float y;
	float z;

	vec3 operator-(const vec3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	vec3 operator-() const {
		return { -x, -y, -z };
	}

	vec3 dot(const vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	vec3 SameDirection(const vec3& other) const {
		return this->dot(other) > 0;
	}

	vec3 cross(const vec3& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}
};

struct vec2 {
	float x;
	float y;

	vec2 operator-(const vec2& other) const {
		return { x - other.x, y - other.y };
	}

	vec2 operator-() const {
		return { -x, -y };
	}

	vec2 dot(const vec2& other) const {
		return x * other.x + y * other.y;
	}

	vec2 SameDirection(const vec2& other) const {
		return this->dot(other) > 0;
	}

	vec2 tripleCross(const vec2& other) const {
		/*vec3 v1 = { x, y, 0 };
		vec3 v2 = { other.x, other.y, 0 };
		vec3 cross1 = v1.cross(v2).cross(v1);
		vec3 cross2 = cross1.cross(v1);*/
		return {
			y*(other.x * y - other.y * x),
			x * (other.y * x - y * other.x)
		};
	}
};

struct Simplex {
private:
	std::array<vec3, 4> points;
	int size = 0;
public:
	Simplex() : size(0) {}

	Simplex& operator=(std::initializer_list<vec3> list) {
		size = list.size();
		std::copy(list.begin(), list.end(), points.begin());
		return *this;
	}

	void push_front(const vec3& point) {
		for (int i = size; i > 0; --i) {
			points[i] = points[i - 1];
		}
		points[0] = point;
		if (size < 4) {
			++size;
		}
	}

	vec3& operator[](int index) {
		return points[index];
	}

	size_t size() const { return size; }

	auto begin() { return points.begin(); }
	auto end() { return points.begin() + size; }
};

struct Collider {
	virtual vec3 FindFurthestPoint(vec3 direction) const = 0;

};

struct MeshCollider : Collider {
private:
	std::vector<vec3> vertices;
public:
	vec3 FindFurthestPoint(vec3 direction) const override {
		vec3 furthestPoint;
		float maxDot = -FLT_MAX;
		for (const vec3& vertex : vertices) {
			float dot = vertex.dot(direction);
			if (dot > maxDot) {
				maxDot = dot;
				furthestPoint = vertex;
			}
		}
		return furthestPoint;
	}
};

vec3 Support(const Collider& colliderA, const Collider& colliderB, vec3 direction) {
	return colliderA.FindFurthestPoint(direction) - colliderB.FindFurthestPoint({ -direction });
}

bool Line(Simplex& simplex, vec3& direction) {
	vec3 a = simplex[0];
	vec3 b = simplex[1];

	vec3 ab = b - a;
	vec3 ao = -a;
	
	if (ab.SameDirection(ao)) {
		direction = ab.cross(ao).cross(ab);
	}
	else {
		simplex = { a };
		direction = ao;
	}

	return false;
}

bool Triange(Simplex& simplex, vec3& direction) {
	vec3 a = simplex[0];
	vec3 b = simplex[1];
	vec3 c = simplex[2];

	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 ao = -a;

	vec3 abc = ab.cross(ac);

	if (abc.cross(ac).SameDirection(ao)) {
		if (ac.SameDirection(ao)) {
			simplex = { a, c };
			direction = ac.cross(ao).cross(ac);
		}
		else {
			return Line(simplex = { a, b }, direction);
		}
	}
	else {
		if (ab.cross(abc).SameDirection(ao)) {
			return Line(simplex = { a, b }, direction);
		}
		else {
			if (abc.SameDirection(ao)) {
				direction = abc;
			}
			else {
				simplex = { a, c, b };
				direction = -abc;
			}
		}
	}
	return false;
}

bool Tetrahedron(Simplex& simplex, vec3& direction) {
	vec3 a = simplex[0];
	vec3 b = simplex[1];
	vec3 c = simplex[2];
	vec3 d = simplex[3];
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 ad = d - a;
	vec3 ao = -a;
	vec3 abc = ab.cross(ac);
	vec3 acd = ac.cross(ad);
	vec3 adb = ad.cross(ab);

	if (abc.SameDirection(ao)) {
		return Triange(simplex = { a, b, c }, direction);
	}
	if (acd.SameDirection(ao)) {
		return Triange(simplex = { a, c, d }, direction);
	}
	if (adb.SameDirection(ao)) {
		return Triange(simplex = { a, d, b }, direction);
	}
	return true; // Origin is inside the tetrahedron

bool NextSimplex(Simplex& simpler, vec3& direction) {
	switch (points.size()) {
		case 2: return Line(points, direction);
		case 3: return Triangle(points, direction);
		case 4: return Tetrahedron(points, direction);
	}
	return false;
}

bool GJK(const Collider& colliderA, const Collider& colliderB) {
	// Initial support point in an arbitrary direction
	vec3 support = Support(colliderA, colliderB, { 1, 0, 0 });

	// Simplex is an array of points that describes the dimension
	Simplex simplex;
	simplex.push_front(support);

	vec3 direction = -support;

	while (true) {
		support = Support(colliderA, colliderB, direction);
		if (dot(support, direction) <= 0) {
			return false; // No collision
		}
		simplex.push_front(support);

		if (NextSimplex(simplex, direction)) {
			return true; // Collision detected
		}
	}
}
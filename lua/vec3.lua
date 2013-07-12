Vec3 = {}
Vec3.__index = Vec3

function Vec3.new(x, y, z)
	local obj = {}

	setmetatable(obj, Vec3)

	if type(x) == 'table' then
		if x[1] and x[2] and x[3] then
			obj.x = x[1]
			obj.y = x[2]
			obj.z = x[3]
		else
			obj.x = x.x
			obj.y = x.y
			obj.z = x.z
		end
	else
		obj.x = x;
		obj.y = y;
		obj.z = z;
	end

	return obj;
end


function Vec3.__add(op1, op2)
	if type(op1)=='table' and type(op2)=='table' then
		return Vec3.new(op1.x + op2.x, op1.y + op2.y, op1.z + op2.z);
	else
		return Vec3.new(op1.x + op2, op1.y + op2, op1.z + op2);
	end
end

function Vec3.__sub(op1, op2)
	if type(op1)=='table' and type(op2)=='table' then
		return Vec3.new(op1.x - op2.x, op1.y - op2.y, op1.z - op2.z);
	else
		return Vec3.new(op1.x - op2, op1.y - op2, op1.z - op2);
	end
end

function Vec3.__div(op1, op2)
	if type(op1)=='table' and type(op2)=='table' then
		return Vec3.new(op1.x / op2.x, op1.y / op2.y, op1.z / op2.z);
	else
		return Vec3.new(op1.x / op2, op1.y / op2, op1.z / op2);
	end
end

function Vec3.__mul(op1, op2)
	if type(op1)=='table' and type(op2)=='table' then
		return Vec3.new(op1.x * op2.x, op1.y * op2.y, op1.z * op2.z);
	else
		return Vec3.new(op1.x * op2, op1.y * op2, op1.z * op2);
	end
end

function Vec3:__tostring()
	return '{' .. self.x .. ', ' .. self.y .. ', ' .. self.z .. '}'
end

function Vec3:normalize()
	return self / self:length();
end

function Vec3.random()
	return Vec3.new(math.random(), math.random(), math.random())
end

function Vec3.cross(x, y)
	return Vec3.new(x.y * y.z - y.y * x.z, x.z * y.x - y.z * x.x, x.x * y.y - y.x * x.y);
end

function Vec3.__eq(op1, op2)
	return op1.x == op2.x and op1.y == op2.y and op1.z == op2.z
end

function Vec3.dot(v1, v2)
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z
end

function Vec3:length(op1)
	return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z);
end

function Vec3.angle(v1, v2)
	if v2 == nil then
		x = v1.x
		z = v1.z

		angle = math.deg( math.atan(z/x) );

		-- 1st quadrant
		if x >= 0 and z >= 0 then
		-- 2nd quadrant
		elseif x < 0 and z >= 0 then
			angle = angle + 180;
		-- 3rd quadrant
		elseif x < 0 and z < 0 then
			angle = angle + 180;
		-- 4th quadrant
		else
			angle = angle + 360;
		end

		return 360-angle;
	else
		return math.deg( math.acos( Vec3.dot(v1, v2) / (v1:length() * v2:length()) ) )
	end
end

Missile = {}
Missile.__index = Missile


function Missile.new(speed, radius, origin, target)
	local self = {}

	setmetatable(self, Missile)

	self.target = target
	self.position = origin
	self.speed = speed
	self.radius = radius
	self.damage = 13

	return self
end

function Missile:update(dt)
	distance = self.target.sceneActor:getPosition() - self.sceneActor:getPosition()
	dir = (distance):normalize()
	distance = distance:length()

	disp = dir * self.speed * dt

	if distance <= self.radius then
		Game:deleteEntity(self)
		self.target.sceneActor:takeDamage(self.damage)
	else
		newPos = self.sceneActor:getPosition() + disp
	
		Scene:setActorPosition(self.sceneActor.sceneHandle, {newPos.x, newPos.y, newPos.z})
	end
end
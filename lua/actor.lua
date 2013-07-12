
require 'mover'

GAME_MODELS = {
	["peasant"] = {
		["model"] = '$ROOT/peasant_pick',
		["skin"] = 'default',
		["anims"] = {
			['walk'] = 'walk',
			['stand'] = 'stand',
			['death'] = 'death'
		},
		['speed'] = 3
	},

	["turret"] = {
		["model"] = '$ROOT/turret',
		["skin"] = 'default',
		["anims"] = {
			['stand'] = 'stand',
			['attack'] = 'attack',
			['death'] = 'death'
		},
		['speed'] = 3
	},
	['missile'] = {
		['model'] = '$ROOT/missile',
		['skin'] = 'default'
	},
	['turret_rider'] = {
		['model'] = '$ROOT/turret_rider',
		['skin'] = 'default',
		['anims'] = {
			['mount'] = 'mount'
		}
	},

}

Actor = {}
Actor.__index= Actor

function Actor.new(sceneHandle)
	local self = {}

	setmetatable(self, Actor)

	self.sceneHandle = sceneHandle

	self.animQueue = {}

	Scene:createActorController(sceneHandle, 1, {height=1, radius=0.3})

	return self
end

function Actor:distance(other)
	return (self:getPosition() - other:getPosition()):length()
end

function Actor:__tostring()
	return '{Actor sceneHandle=' .. self.sceneHandle .. '}'
end

function Actor:getPosition()
	return Vec3.new( Scene:getActorTransform(self.sceneHandle).position )
end

function Actor.__eq(op1, op2)
	return op1.id == op2.id
end

function Actor:faceActor(actor)
	dir = actor:getPosition() - self:getPosition()
	dir.y = 0
	dir = dir:normalize()
	
	facing = Vec3.cross(dir, Vec3.new(0, 1, 0))

	self:face(facing)
end

function Actor:setAnimQueue(queue)
	self.animQueue = queue
	self:playAnim(self.animQueue[1], false)
	table.remove(self.animQueue, 1)
end

function Actor:setModel(name, skin)
	Scene:setActorModel(self.sceneHandle, name, skin)
end

function Actor:clearAnimQueue()
	self.animQueue = nil
end

function Actor:update(dt)
	if self.animQueue ~= nil then
		if not Scene:isActorAnimationPlaying(self.sceneHandle) then
			loop = table.getn(self.animQueue) == 1
			self:playAnim(self.animQueue[1], loop)
			table.remove(self.animQueue, 1)
		end
	end
end

function Actor:setPosition(pos)
	Scene:setActorPosition(self.sceneHandle, {pos.x, pos.y, pos.z})
end

function Actor:playAnim(name, loop)
	if loop == nil then loop = true end

	Scene:playActorAnimation(self.sceneHandle, name, loop)
end

function Actor:face(dir)
	Scene:setActorFacing(self.sceneHandle, {dir.x, dir.y, dir.z})
end


function Actor:inRegion(pos, radius)
	actors = Physics:getActorsInRegion(pos, radius);

	for c, actor in pairs(actors) do
		if(self.sceneHandle == actor) then
			return true;
		end
	end

	return false;
end
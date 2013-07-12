ActorMover = {}
ActorMover.__index= ActorMover

function ActorMover.new(actor)
	local obj = {}

	setmetatable(obj, ActorMover)

	obj.actor = actor
	obj.destination = nil
	obj.isMoving = false
	obj.callback = nil
	obj.speed = 5

	return obj
end

function ActorMover:moveTo(pos)
	self.destination = pos
	self.isMoving = true
end

function ActorMover:stopMoving(pos)
	self.destination = nil
	self.isMoving = false
end

function ActorMover:update(dt)
	if self.isMoving then
		position = self.actor:getPosition()

		distance = (self.destination - position):length()

		if distance < 1 then
			self.isMoving = false
			self.destination = nil

			if self.callback then
				self.callback(self.actor)
			end

			return
		end
	end

	if self.destination then
		moveDir = (self.destination - position)
		moveDir.y = 0
		moveDir = moveDir:normalize()

		moveDisp = moveDir*self.speed*dt
		moveDisp.y = -9.18*dt

		Scene:moveActor(self.actor.sceneHandle,
				{moveDisp.x, moveDisp.y, moveDisp.z}, dt)


		facing = Vec3.cross(moveDir, Vec3.new(0, 1, 0))
		Scene:setActorFacing(self.actor.sceneHandle, {facing.x, facing.y, facing.z} )

	else
		Scene:moveActor(self.actor.sceneHandle,
				{0, -9.18*dt, 0}, dt)
	end

	
end
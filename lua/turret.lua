Turret = {}
Turret.__index = Turret

function Turret.new()
	local self = {}
	setmetatable(self, Turret)
	self.target = nil
	self.timeAccomulator = TimeAccomulator.new(5)
	self.radius= 10
	self.attackTimer = 0
	self.attackSpeed = 1

	self.flag = 0
	self.rider = {
		update = function(dt) end
	}

	Game:insertEntity(self.rider, 'turret_rider',  Vec3.new(200, 3, 200), GameTeams.eTEAM_1)
	 

	return self
end


function Turret:scan()
	pos = self.sceneActor:getPosition()
	actors = Physics:getActorsInRegion({pos.x, pos.y, pos.z}, self.radius, 1)

	for c, id in pairs(actors) do
		if id ~= self.sceneActor.sceneHandle then
			gameActor = Game.actors[Scene:getActorUserData(id)]

			if gameActor.sceneActor.team ~= self.sceneActor.team and gameActor.sceneActor.isAlive then
				return gameActor
			end
		end
	end

	return nil
end



function Turret.update(self, dt)
	if self.flag == 0 then
		print('ateawch')
		 Scene:attachActor(self.rider.sceneActor.sceneHandle,
			self.sceneActor.sceneHandle, 'Bone_' .. 16)

		self.rider.sceneActor:playAnim('mount', true)

		self.flag = 1
	end
	
	self.sceneActor:update(dt)


	if self.target == nil  then
		self.target = self:scan()
		if self.target then
			LOG('Target acquired')
			EventManager:queueEvent('OnTargetAcquired', {})
		end
	elseif self.target ~= nil then
		if not self.target.sceneActor.isAlive then
			self.target = nil
			self.sceneActor:face(Vec3.new(0, 0, -1))
			EventManager:queueEvent('OnTargetLost', {})
			return
		end	

		distance = (self.target.sceneActor:getPosition() - self.sceneActor:getPosition()):length()
		if distance > self.radius+5 then
			LOG('Target lost, scanning...')
			self.target = nil
			self.sceneActor:face(Vec3.new(0, 0, -1))
			EventManager:queueEvent('OnTargetLost', {})
		else
			self.sceneActor:faceActor(self.target.sceneActor)

			self.attackTimer = self.attackTimer-dt

			-- attacking
			if self.attackTimer <= 0 then
				self.attackTimer = 1/self.attackSpeed

				-- attack
				
				self.sceneActor:setAnimQueue({'attack', 'stand'})

				missilePos= self.sceneActor:getPosition() + Vec3.new(0, 3, 0)
				missile = Missile.new(15, 2, missilePos, self.target)
				Game:insertEntity( missile, 'missile',  missilePos, GameTeams.eTEAM_1)
			end
		end
	end
	
end
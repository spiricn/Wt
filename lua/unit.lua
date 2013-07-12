Unit = {}
Unit.__index = Unit


UnitType = {
	eMELEE = 0,
	eRANGED = 1
}

UnitState = {
	eIDLE = 0,
	eATTACKING = 1,
	eATTACK_MOVE = 2,
	eDEAD = 3,
}

UnitStance = {
	ePASSIVE = 0,
	eDEFFENSIVE = 1,
	eAGGRESIVE = 2
}

turretDesc  = {
	model = '$ROOT/turret',
	skin = 'default',
	type = UnitType.eRANGED,
	moveSpeed = 0,
	attackSpeed = 1,
	aggroRadius = 200,
	maxHitPoints = 10000,
	attackRange = 100,
	damage = 33,
	decayTime=5,
	animations = {
		stand = 'stand',
		attack = 'attack',
		death = 'death',
		walk = 'stand'
	},
	mountable = true,
	mountBone = 'Bone_16',
	sounds = {
		attack = '$ROOT/turret/attack'
	}
}

ghoul = {
	model = '$ROOT/ghoul',
	skin = 'white',
	type = UnitType.eMELEE,
	moveSpeed = 5,
	decayTime=9,
	mountable = false,
	attackSpeed = 0.3,
	aggroRadius = 20,
	maxHitPoints = 100,
	damage = 33,
	attackRange = 2,
	animations = {
		stand = 'stand',
		walk = 'walk',
		attack = 'attack',
		death = 'death'
	},
	sounds = {
		attack = '$ROOT/ghoul/attack',
		aggro = '$ROOT/ghoul/aggro',
		wound = '$ROOT/ghoul/wound',
		death = '$ROOT/ghoul/death',
	}
}

function Unit:scan()
	pos = self.sceneActor:getPosition()

	actors = Physics:getActorsInRegion({pos.x, pos.y, pos.z}, self.unitDesc.aggroRadius, 1)

	res = {}

	for c, id in pairs(actors) do
		if id ~= self.sceneActor.sceneHandle then
			unit = Game.actors[Scene:getActorUserData(id)]

			if unit.team ~= self.team and unit.isAlive then
				table.insert(res, unit)
			end
		end
	end

	return res
end


function Unit.new(team, position, unitDesc)
	local self = {}
	setmetatable(self, Unit)

	self.target = nil
	self.team = team
	self.attackTimer = 0
	self.unitDesc = unitDesc
	self.isAlive = 0
	self.hitPoints = self.unitDesc.maxHitPoints
	self.state = UnitState.eIDLE
	self.stance = UnitStance.eAGGRESIVE
	
	Game:addUnit(self)

	self.sceneActor:setModel(self.unitDesc.model, self.unitDesc.skin)

	self.sceneActor:setPosition( Vec3.new( position ) )
	self.sceneActor:playAnim(self.unitDesc.animations.stand, true)

	self.mover = ActorMover.new(self.sceneActor)
	self.mover.speed = self.unitDesc.moveSpeed

	self.deathTimer = self.unitDesc.decayTime

	return self
end

function Unit:getClosestTarget()
	targets = self:scan()
	closestDistance = 0
	closestTarget = nil

	for idx, target in pairs(targets) do
		distance = self.sceneActor:distance(target.sceneActor)
		if  closestTarget == nil or distance < closestDistance then
			closestDistance = distance
			closestTarget = target
		end
	end

	return closestTarget
end

time = 0

function Unit:update(dt)
	self.sceneActor:update(dt)

	if self.isAlive then
		self.mover:update(dt)
	end


	if self.state == UnitState.eIDLE then
		if self.stance == UnitStance.eAGGRESIVE then
			newTarget = self:getClosestTarget()

			if newTarget then
				self:onTargetAcquired(newTarget)
				return
			end
		end

	elseif self.state == UnitState.eATTACKING then
		
		distance = (self.target.sceneActor:getPosition() - self.sceneActor:getPosition()):length()

		if distance >= self.unitDesc.aggroRadius + 5 then
			self:onTargetOutOfRange()
			return
		end

		if not self.target.isAlive then
			self:onTargetDied()
			return
		end

		if distance > self.unitDesc.attackRange then
			self.state = UnitState.eATTACK_MOVE
			self.sceneActor:clearAnimQueue()
			self.sceneActor:playAnim(self.unitDesc.animations['walk'], true)
			return
		end

		self.sceneActor:faceActor(self.target.sceneActor)
		

		self.attackTimer = self.attackTimer-dt

		if self.attackTimer <= 0 then
			self.attackTimer = 1/self.unitDesc.attackSpeed
			self:onPreformAttack()
		end

	elseif self.state == UnitState.eATTACK_MOVE then
		if not self.target.isAlive then
			self:onTargetDied()
			return
		end

		
		self.mover:moveTo( self.target.sceneActor:getPosition() )

		distance = (self.target.sceneActor:getPosition() - self.sceneActor:getPosition()):length()

		if distance <= self.unitDesc.attackRange then
			self.state = UnitState.eATTACKING
			self.mover:stopMoving()
			self.sceneActor:clearAnimQueue()
			self.sceneActor:playAnim(self.unitDesc.animations['stand'], true)
		end
	end
end

function Unit:onPreformAttack()
	self.sceneActor:setAnimQueue({self.unitDesc.animations['attack'], self.unitDesc.animations['stand']})
	self.target:onTakeDamage(self, self.unitDesc.damage)
	pos = self.sceneActor:getPosition()
	Sound:playSound(self.unitDesc.sounds.attack, {pos.x, pos.y, pos.z})
end

function Unit:onTakeDamage(origin, damage)
	if self.stance == UnitStance.eDEFFENSIVE and self.target == nil then
		self:onTargetAcquired(origin)
	end

	self.hitPoints = self.hitPoints - damage
	if self.hitPoints <= 0 then
		self.hitPoints = 0
		self:onDeath()
	else
		if self.unitDesc.sounds.wound ~= nil then Sound:playSound(self.unitDesc.sounds.wound, {pos.x, pos.y, pos.z}) end
	end
end

function Unit:onDeath()
	self.state = UnitState.eDEAD
	self.isAlive = false
	self.sceneActor:clearAnimQueue()
	self.sceneActor:playAnim(self.unitDesc.animations['death'], false)
	Scene:destroyController(self.sceneActor.sceneHandle)
	if self.unitDesc.sounds.death ~= nil then Sound:playSound(self.unitDesc.sounds.death, {pos.x, pos.y, pos.z}) end

	EventManager:queueEvent('OnUnitDied', self)
end

function Unit:onTargetDied()
	self.target = nil
	self.state = UnitState.eIDLE
	self.sceneActor:clearAnimQueue()
	self.sceneActor:playAnim(self.unitDesc.animations['stand'], true)
	self.mover:stopMoving()
end

function Unit:__tostring()
	return '{Unit id=' .. self.id .. '}'
end

function Unit:onTargetOutOfRange()
	self.state = UnitState.eIDLE
end

function Unit:onTargetAcquired(target)
	self.state = UnitState.eATTACK_MOVE
	self.sceneActor:clearAnimQueue()
	
	self.sceneActor:playAnim(self.unitDesc.animations['walk'], true)
	self.target = target
	if self.unitDesc.sounds.aggro ~= nil then Sound:playSound(self.unitDesc.sounds.aggro, {pos.x, pos.y, pos.z}) end
end

function Unit:orderMove(pos)
	self.mover:moveTo(pos)
end
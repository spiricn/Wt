TimeAccomulator = {}
TimeAccomulator.__index = TimeAccomulator

function TimeAccomulator.new(time)
	local self = {}
	setmetatable(self, TimeAccomulator)
	self._time = time
	self.currTime = 0.0

	return self
end

function TimeAccomulator:update(dt)
	self.currTime = self.currTime + dt
	if self.currTime > self._time then
		self.currTime = self.currTime - self._time
		return true
	end

	return false
end
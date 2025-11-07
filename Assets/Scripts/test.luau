print("Hello From ZED::Scripting::Luau")

local acc = 0
local sec = 0

return
{
	OnStart = function(self)
		print("OnStart entity:", self.entity)
	end,

	OnUpdate = function(self, dt)
		--[[
		acc += dt
		while acc >= 1.0 do
			sec += 1
			print(("Tick @ %d"):format(sec))
			acc -= 1.0
		end
		]]
	end,
}
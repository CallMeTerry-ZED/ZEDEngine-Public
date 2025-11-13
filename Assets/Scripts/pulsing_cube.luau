-- Example: Pulsing scale animation using time
-- Demonstrates: Transform.Scale, GetTransform, SetTransform, GetElapsedTime

local baseScale = 1.0
local pulseSpeed = 2.0  -- pulses per second
local pulseAmount = 0.3  -- scale variation

return
	{
		OnStart = function(self)
			print("PulsingCube script started for entity: ", self.entity)
			-- Get initial scale
			local tr = ZED.GetTransform(self.entity)
			if tr and tr.scale then
				baseScale = tr.scale.x
			end
		end,

		OnUpdate = function(self, dt)
			local time = ZED.GetElapsedTime()
			local pulse = math.sin(time * pulseSpeed * math.pi * 2.0) * pulseAmount
			local scale = baseScale + pulse

			-- Set scale uniformly
			ZED.SetTransform(self.entity, nil, nil, {x = scale, y = scale, z = scale})
		end,
	}
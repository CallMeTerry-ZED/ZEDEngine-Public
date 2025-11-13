-- Example: Rotating a cube around Y axis
-- Demonstrates: Transform.Rotate, GetDeltaTime

local spinSpeed = 1.0  -- radians per second

return
	{
		OnStart = function(self)
			print("SpinningCube script started for entity: ", self.entity)
		end,

		OnUpdate = function(self, dt)
		-- Rotate around Y axis
			ZED.Transform.Rotate(self.entity, 0.0, spinSpeed * dt, 0.0)
		end,
	}
-- Example: Moving a cube using input
-- Demonstrates: Transform.Translate, IsKeyDown, GetDeltaTime, Key constants

local moveSpeed = 5.0  -- units per second

return
	{
		OnStart = function(self)
			print("MovingCube script started for entity: ", self.entity)
		end,

		OnUpdate = function(self, dt)
			local speed = moveSpeed * dt
			local moveX = 0.0
			local moveZ = 0.0

			-- WASD movement
			if ZED.IsKeyDown(ZED.Key.W) then
				moveZ = moveZ - speed
			end
			if ZED.IsKeyDown(ZED.Key.S) then
				moveZ = moveZ + speed
			end
			if ZED.IsKeyDown(ZED.Key.A) then
				moveX = moveX - speed
			end
			if ZED.IsKeyDown(ZED.Key.D) then
				moveX = moveX + speed
			end

			-- Apply movement
			if moveX ~= 0.0 or moveZ ~= 0.0 then
				ZED.Transform.Translate(self.entity, moveX, 0.0, moveZ)
			end
		end,
	}
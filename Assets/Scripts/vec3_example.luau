-- Example: Vec3 usage
-- Demonstrates: Vec3.new, Vec3 operations (add, sub, mul, div)

return
	{
		OnStart = function(self)
			print("Vec3Example script started for entity: ", self.entity)

			-- Create Vec3 instances
			local v1 = ZED.Vec3.new(1.0, 2.0, 3.0)
			local v2 = ZED.Vec3.new(4.0, 5.0, 6.0)

			-- Vec3 operations
			local v3 = v1 + v2  -- addition
			local v4 = v2 - v1  -- subtraction
			local v5 = v1 * 2.0  -- scalar multiplication
			local v6 = v2 / 2.0  -- scalar division

			print("v1: ", tostring(v1))
			print("v2: ", tostring(v2))
			print("v1 + v2: ", tostring(v3))
			print("v2 - v1: ", tostring(v4))
			print("v1 * 2: ", tostring(v5))
			print("v2 / 2: ", tostring(v6))

			-- Access components
			print("v1.x: ", v1.x)
			print("v1.y: ", v1.y)
			print("v1.z: ", v1.z)
		end,

		OnUpdate = function(self, dt)
		-- Example: Use Vec3 for movement
			local direction = ZED.Vec3.new(0.0, 0.0, -1.0)
			local speed = 2.0
			local movement = direction * (speed * dt)

		-- Apply movement (uncomment to use)
		-- ZED.Transform.Translate(self.entity, movement.x, movement.y, movement.z)
		end,
	}
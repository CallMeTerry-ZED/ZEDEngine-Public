-- Example: Comprehensive transform manipulation
-- Demonstrates: GetTransform, SetTransform, Transform operations

return
	{
		OnStart = function(self)
			print("TransformExample script started for entity: ", self.entity)

			-- Get and print current transform
			local tr = ZED.GetTransform(self.entity)
			if tr then
				print("Position: ", tr.position.x, ", ", tr.position.y, ", ", tr.position.z)
				print("Rotation: ", tr.rotation.x, ", ", tr.rotation.y, ", ", tr.rotation.z)
				print("Scale: ", tr.scale.x, ", ", tr.scale.y, ", ", tr.scale.z)
			end
		end,

		OnUpdate = function(self, dt)
		-- Example: Rotate on all axes at different speeds
			ZED.Transform.Rotate(self.entity, dt * 0.5, dt * 1.0, dt * 0.75)

		-- Example: Set position directly
		-- ZED.SetTransform(self.entity, {x = 0.0, y = math.sin(ZED.GetElapsedTime()), z = 0.0})
		end,
	}
-- Example: Camera manipulation
-- Demonstrates: GetCamera, SetCamera

return
	{
		OnStart = function(self)
			print("CameraExample script started for entity: ", self.entity)

			-- Get camera component
			local cam = ZED.GetCamera(self.entity)
			if cam then
				print("Camera FOV: ", cam.fovRadians)
				print("Camera Aspect: ", cam.aspect)
				print("Camera Primary: ", cam.primary)
			end
		end,

		OnUpdate = function(self, dt)
		-- Example: Adjust FOV based on input
		-- local cam = ZED.GetCamera(self.entity)
		-- if cam then
		--     if ZED.IsKeyDown(ZED.Key.LeftShift) then
		--         cam.fovRadians = cam.fovRadians + dt * 0.1
		--     end
		--     if ZED.IsKeyDown(ZED.Key.LeftControl) then
		--         cam.fovRadians = cam.fovRadians - dt * 0.1
		--     end
		--     ZED.SetCamera(self.entity, cam)
		-- end
		end,
	}
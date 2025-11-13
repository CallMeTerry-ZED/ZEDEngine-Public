-- Example: Input checking
-- Demonstrates: IsKeyDown, Key constants

return
	{
		OnStart = function(self)
			print("InputExample script started for entity: ", self.entity)
		end,

		OnUpdate = function(self, dt)
		-- Check various keys
			if ZED.IsKeyDown(ZED.Key.W) then
				print("W key is pressed")
			end
			if ZED.IsKeyDown(ZED.Key.Space) then
				print("Space key is pressed")
			end
			if ZED.IsKeyDown(ZED.Key.MouseLeft) then
				print("Left mouse button is pressed")
			end
			if ZED.IsKeyDown(ZED.Key.Escape) then
				print("Escape key is pressed")
			end
		end,
	}
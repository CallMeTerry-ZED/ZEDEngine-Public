-- Example: Time usage
-- Demonstrates: GetDeltaTime, GetElapsedTime

local lastPrintTime = 0.0
local printInterval = 1.0  -- print every second

return
	{
		OnStart = function(self)
			print("TimeExample script started for entity: ", self.entity)
			lastPrintTime = ZED.GetElapsedTime()
		end,

		OnUpdate = function(self, dt)
			local elapsed = ZED.GetElapsedTime()

			-- Print time info every second
			if elapsed - lastPrintTime >= printInterval then
				print("Elapsed Time: ", elapsed, " seconds")
				print("Delta Time: ", dt, " seconds")
				print("FPS (approx): ", 1.0 / dt)
				lastPrintTime = elapsed
			end
		end,
	}
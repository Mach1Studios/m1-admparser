local path = "PATH_TO/AtmosParser/ReaperScript"
local filename = "my_example.atmos.metadata"

function readAll(file)
    local f = assert(io.open(file, "rb"))
    local content = f:read("*all")
    f:close()
    return content
end

function msg(m)
  return reaper.ShowConsoleMsg(tostring(m) .. "\n")
end

function map_range( a1, a2, b1, b2, s )
    return b1 + (s-a1)*(b2-b1)/(a2-a1)
end

package.path = path .. '?.lua;' .. package.path

local inspect = require('inspect')
local yaml = require('yaml')

local data = yaml.eval(readAll(path .. filename))
local ID = "0"

--msg(inspect(data.events))

function vector_dot(a, b)
  return a[1] * b[1] + a[2] * b[2] + a[3] * b[3]
end

function vector_mag(a)
  return math.sqrt(a[1] * a[1] + a[2] * a[2] + a[3] * a[3])
end

function vector_angle_between(a, b)
  return math.deg(math.acos(vector_dot(a, b) / (vector_mag(a) * vector_mag(b))))
end

function insert_env_points()
  msg("Checking FX")
  local retval, track_number, item_number, fx_number = reaper.GetFocusedFX()
  if retval == 1 and item_number == -1 then -- currently only track FXs are supported
    local track = reaper.CSurf_TrackFromID(track_number, false)
    local is_open = reaper.TrackFX_GetOpen(track, fx_number) 
    if not is_open then
      msg("Need to open FX UI")
      return
    end
    reaper.PreventUIRefresh(1)
    msg("Start")
	local env = {}
    for i=1, reaper.TrackFX_GetNumParams(track, fx_number) do
      local ret, param_name = reaper.TrackFX_GetParamName(track, fx_number, i-1, "")
      local fx_env = reaper.GetFXEnvelope(track, fx_number, i-1, true)
      if fx_env ~= nil then
		if param_name == "Rotation" then
		  env["Rotation"]  = fx_env
		end
		if param_name == "Diverge" then
		  env["Diverge"]  = fx_env
		end
		if param_name == "Elevation" then
		  env["Elevation"]  = fx_env
		end
		if param_name == "x" then
		  env["x"]  = fx_env
		end
		if param_name == "y" then
		  env["y"]  = fx_env
		end
      end
    end
	
	-- clear envelope
	reaper.DeleteEnvelopePointRange(env["Rotation"], 0, 3600)
	reaper.DeleteEnvelopePointRange(env["Diverge"], 0, 3600)
	reaper.DeleteEnvelopePointRange(env["Elevation"], 0, 3600)
	reaper.DeleteEnvelopePointRange(env["x"], 0, 3600)
	reaper.DeleteEnvelopePointRange(env["y"], 0, 3600)

	local found = false
	local cnt = 0
	for i = 1, #data.events do

		--msg(inspect(data.events[i]))
		--msg(data.events[i].ID .. "  " .. ID)

		if data.events[i].ID ~= nil and data.events[i].ID ~= ID then
			found = false
		end

		if data.events[i].ID ~= nil and data.events[i].ID == ID then
			found = true
		end

		if found and data.events[i].pos ~= nil then
			local p = 1.0 * data.events[i].samplePos / data.sampleRate
			local x = (data.events[i].pos[1])
			local y = (data.events[i].pos[2])
			local z = (data.events[i].pos[3])
			
			local Rotation = math.deg(math.atan(x, y))
			Rotation = map_range(-180, 180, 0.0, 1.0, Rotation)
			--msg("Rotation: " .. Rotation)
			
			local Diverge = math.sqrt(x * x + y * y) / math.sqrt(2.0)
			Diverge = map_range(-1.0, 1.0, 0.0, 1.0, Diverge)
			--msg("Diverge: " .. Diverge)
			
			local Elevation = vector_angle_between({x, y, z}, {x, y, 0.0})
			Elevation = map_range(-90.0, 90.0, 0.0, 1.0, Elevation)
			--msg("Elevation: " .. Elevation)
			
			reaper.InsertEnvelopePoint(env["Rotation"], p, Rotation, 0, 0, false, true)
			reaper.InsertEnvelopePoint(env["Diverge"], p, Diverge, 0, 0, false, true)
			reaper.InsertEnvelopePoint(env["Elevation"], p, Elevation, 0, 0, false, true)
			
			cnt = cnt + 1
		end

	end
    msg("Imported " .. cnt .. " points")
	
    reaper.PreventUIRefresh(-1)
    reaper.TrackList_AdjustWindows(false)
  else 
	msg("No FX, you need to open M1-Panner UI window")
  end
  reaper.Undo_OnStateChangeEx("Create envelope points from FX parameter values", -1, -1)

  msg("Finished")
end


local retval, val = reaper.GetUserInputs("Choose track ID", 1, "ID", "ATU_00000001")
msg("-------------")
if retval then 
	ID = val --tonumber(val)
  msg(inspect(ID))
	reaper.defer(insert_env_points)
end

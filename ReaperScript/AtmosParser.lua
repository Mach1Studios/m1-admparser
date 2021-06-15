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
  --msg("Checking FX")
  local data1 = yaml.eval(readAll(path .. filename))
  local data2 = yaml.eval(readAll(path .. filename .. ".metadata"))
  --msg(inspect(data.events))
 
  for n=0, reaper.GetNumTracks()-1 do
    local track = reaper.CSurf_TrackFromID( reaper.GetNumTracks(n), false )
	local retval, trackName = reaper.GetTrackName(track)
    msg("process track: " .. trackName)

    reaper.PreventUIRefresh(1)

	local env = {}
	local fx_number = 0
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
	
	for i = 1, #data1.presentations[1].bedInstances[1].channels do

		local ID = tostring(data1.presentations[1].bedInstances[1].channels[i].ID)

		if ID ~= nil and ID == trackName then
		
			local Rotation = 0
			local Diverge = 0
			local Elevation = 0

			local channel = data1.presentations[1].bedInstances[1].channels[i].channel
			if channel == "L" then
				Rotation = -90
				Diverge = 100
				Elevation = 0
			elseif channel == "R" then
				Rotation = 90
				Diverge = 100
				Elevation = 0
			elseif channel == "C" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "LFE" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Lss" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Rss" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Lrs" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Rrs" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Lts" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			elseif channel == "Rts" then
				Rotation = 0
				Diverge = 100
				Elevation = 0
			end

			Rotation = map_range(-180, 180, 0.0, 1.0, Rotation)
			Diverge = map_range(-100, 100, 0.0, 1.0, Diverge)
			Elevation = map_range(-90.0, 90.0, 0.0, 1.0, Elevation)

			reaper.InsertEnvelopePoint(env["Rotation"], 0, Rotation, 0, 0, false, true)
			reaper.InsertEnvelopePoint(env["Diverge"], 0, Diverge, 0, 0, false, true)
			reaper.InsertEnvelopePoint(env["Elevation"], 0, Elevation, 0, 0, false, true)

			cnt = cnt + 1
		end
	end
	
	for i = 1, #data2.events do

		--msg(inspect(data2.events[i]))
		--msg(data2.events[i].ID .. "  " .. trackName)
		local ID = tostring(data2.events[i].ID)

		if ID ~= nil and ID ~= trackName then
			found = false
		end

		if ID ~= nil and ID == trackName then
			found = true
		end

		if found and data2.events[i].pos ~= nil then
			local p = 1.0 * data2.events[i].samplePos / data2.sampleRate
			local x = (data2.events[i].pos[1])
			local y = (data2.events[i].pos[2])
			local z = (data2.events[i].pos[3])
			
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
	
  end
  reaper.Undo_OnStateChangeEx("Create envelope points from FX parameter values", -1, -1)

  msg("Finished")
end


reaper.defer(insert_env_points)

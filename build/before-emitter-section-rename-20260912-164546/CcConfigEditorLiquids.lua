-- Removes leading and trailing whitespace from a value.
local function trim(value)
    return (value or ""):gsub("^%s+", ""):gsub("%s+$", "")
end

-- Parses INI text into section and key tables.
local function read_ini(content)
    local result = {}
    local section = ""

    -- Returns an empty table when no valid INI text was loaded.
    if type(content) ~= "string" then return result end

    -- Reads the INI one line at a time.
    for line in content:gmatch("[^\r\n]+") do
        local section_name = line:match("^%s*%[([^%]]+)%]%s*$")

        -- Creates a table when a new INI section is found.
        if section_name then
            section = section_name:lower()
            result[section] = result[section] or {}
        
        -- Stores key-value pairs under the current INI section.
        else
            local key, value = line:match("^%s*([%w_]+)%s*=%s*(.-)%s*$")
            if key and result[section] then
                result[section][key:lower()] = trim(value)
            end
        end
    end

    -- Returns all parsed INI values.
    return result
end

-- Loads and parses the same Liquids configuration file used by the DLL.
local ini_content = file_load2("Extensions/Liquids/Config.ini")
local ini = read_ini(ini_content)

-- Reads an INI value or returns its fallback.
local function ini_value(section, key, fallback)
    local group = ini[section:lower()]
    local value = group and group[key:lower()] or nil
    if value == nil or value == "" then return fallback end
    return value
end

-- Reads an INI value as a boolean.
local function ini_bool(section, key, fallback)
    local value = ini_value(section, key, fallback and "true" or "false"):lower()
    return value == "1" or value == "true" or value == "yes" or value == "on"
end

-- Reads an INI value as a number.
local function ini_number(section, key, fallback)
    return tonumber(ini_value(section, key, tostring(fallback))) or fallback
end

-- Escapes and quotes text for generated BS code.
local function quote(value)
    local text = tostring(value or "")
    text = text:gsub("\\", "\\\\"):gsub('"', '\\"')
    return '"' .. text .. '"'
end

-- Converts a Lua string list into a BS string array.
local function string_array(values)
    local rendered = {}
    for index, value in ipairs(values) do rendered[index] = quote(value) end
    return table.concat(rendered, ", ")
end

-- Formats a number as a BS integer or float value.
local function typed_number(kind, value)
    if kind == "I32" then
        return "I32(" .. tostring(math.floor(value + 0.5)) .. ")"
    end
    return "F32(" .. string.format("%.9g", value) .. ")"
end

-- The DLL inserts installed preset-folder names directly into these native
-- CustomParameter arrays before TK17 builds the settings controls.
local ejaculationPresetLabels = { "False" }
local ejaculationPresetValues = { "false" }

-- Defines the ordered GUI controls and their INI bindings.
local controls = {
    { 
        type="spin", 
        name="NcLiquidsMasterSwitch", 
        description="Enable (Requires Restart):", 
        icon=201, 
        labels={"Off", "On"}, 
        values={"OFF", "ON"}, 
        section="liquids", 
        key="enabled", 
        fallback=true 
    },
    { 
        type="header", 
        name="NcLiquidsPhysicsHeader", 
        description="Physics:", 
        icon=201 
    },
    { 
        type="slider", 
        name="NcLiquidsPhysicsSpeed", 
        description="Launch Speed:", 
        labels={"Close", "Far"}, 
        number="F32", 
        min=1.0, 
        max=5.0, 
        section="liquid_physics", 
        key="speed", 
        fallback=3.0 
    },
    {
        type="slider",
        name="NcLiquidsPhysicsSpread",
        description="Emission Spread:",
        labels={"Narrow", "Wide"},
        number="F32",
        min=0.0,
        max=0.20,
        section="liquid_physics",
        key="spread",
        fallback=0.05
    },
    {
        type="slider",
        name="NcLiquidsPhysicsDrag",
        description="Air Drag:",
        labels={"Low", "High"},
        number="F32",
        min=0.0,
        max=17.5,
        section="liquid_physics",
        key="drag",
        fallback=0.08
    },
    {
        type="slider",
        name="NcLiquidsPhysicsGravity",
        description="Gravity Strength:",
        labels={"Low", "High"},
        number="F32",
        min=0.0,
        max=5.0,
        section="liquid_physics",
        key="gravity_strength",
        fallback=1.0
    },
    {
        type="header",
        name="NcLiquidsAppearanceHeader",
        description="Appearance:",
        icon=201
    },
    {
        type="slider",
        name="NcLiquidsAmount",
        description="Liquid Amount:",
        labels={"Less Liquid", "More Liquid"},
        number="F32",
        min=10.0,
        max=240.0,
        section="liquids",
        key="spawn_rate",
        fallback=100.0
    },
    {
        type="slider",
        name="NcLiquidsStreamCohesion",
        description="Stream Cohesion:",
        labels={"Short Stream", "Long Stream"},
        number="F32",
        min=0.02,
        max=2.0,
        section="liquids",
        key="model_stream_cohesion",
        fallback=0.20
    },
    {
        type="slider",
        name="NcLiquidsDropletAmount",
        description="Separate Droplet Chance:",
        labels={"Mostly Stream", "More Separate Droplets"},
        number="F32",
        min=0.0,
        max=0.75,
        section="liquids",
        key="model_satellite_chance",
        fallback=0.50
    },
    {
        type="slider",
        name="NcLiquidsVisualSize",
        description="Liquid Size:",
        labels={"Small", "Large"},
        number="F32",
        min=0.001,
        max=0.020,
        section="liquid_visuals",
        key="size",
        fallback=0.005
    },
    {
        type="slider",
        name="NcLiquidsStreamThickness",
        description="Stream Thickness:",
        labels={"Thin", "Thick"},
        number="F32",
        min=0.4,
        max=3.0,
        section="liquid_visuals",
        key="stream_thickness",
        fallback=1.0
    },
    {
        type="slider",
        name="NcLiquidsVisualOpacity",
        description="Liquid Opacity:",
        labels={"Transparent", "Opaque"},
        number="F32",
        min=0.0,
        max=1.0,
        section="liquid_visuals",
        key="stream_opacity",
        fallback=0.50
    },
    {
        type="header",
        name="NcLiquidsPerformanceHeader",
        description="Performance:",
        icon=201
    },
    {
        type="slider",
        name="NcLiquidsParticleLimit",
        description="Maximum Particles:",
        labels={"Better Performance", "More Liquid"},
        number="F32",
        min=32,
        max=512,
        section="liquids",
        key="particle_limit",
        fallback=150
    },
    {
        type="slider",
        name="NcLiquidsStreamQuality",
        description="Stream Quality:",
        labels={"Performance", "Smooth"},
        number="F32",
        min=1,
        max=12,
        section="liquid_visuals",
        key="stream_curve_smoothness",
        fallback=12
    },
    {
        type="header",
        name="NcLiquidsPulseHeader",
        description="Pulse settings - Control the number, timing, and strength of liquid pulses",
        icon=201
    },
    {
        type="slider",
        name="NcLiquidsPulseCount",
        description="Pulse Count:",
        labels={"Fewer Pulses", "More Pulses"},
        number="F32",
        min=1,
        max=10,
        section="liquids",
        key="model_pulse_count",
        fallback=6
    },
    {
        type="slider",
        name="NcLiquidsPulseDuration",
        description="Pulse Duration:",
        labels={"Short", "Long"},
        number="F32",
        min=0.1,
        max=2.0,
        section="liquids",
        key="model_pulse_duration",
        fallback=0.5
    },
    {
        type="slider",
        name="NcLiquidsPulseInterval",
        description="Pulse Interval:",
        labels={"Short Gap", "Long Gap"},
        number="F32",
        min=0.0,
        max=3.0,
        section="liquids",
        key="model_pulse_interval",
        fallback=1.0
    },
    {
        type="slider",
        name="NcLiquidsPulseDecay",
        description="Pulse Decay:",
        labels={"Consistant", "Fades Quickly"},
        number="F32",
        min=0.0,
        max=0.10,
        section="liquids",
        key="model_pulse_decay",
        fallback=0.10
    },
    {
        type="header",
        name="NcLiquidsCollisionHeader",
        description="Collision and Stains:",
        icon=201
    },
    {
        type="spin",
        name="NcLiquidsCollisionEnabled",
        description="Liquid Collision:",
        labels={"Off", "On"},
        values={"OFF", "ON"},
        section="liquid_collision",
        key="enabled",
        fallback=true
    },
    {
        type="spin",
        name="NcLiquidsBodyStains",
        description="Body Stains:",
        labels={"Off", "On"},
        values={"OFF", "ON"},
        section="liquid_collision",
        key="spawn_model_stains",
        fallback=true
    },
    {
        type="spin",
        name="NcLiquidsAnimatedStains",
        description="Animated Stains:",
        labels={"Fixed", "Dripping"},
        values={"OFF", "ON"},
        section="liquid_collision",
        key="native_decal_drip",
        fallback=false
    },
    {
        type="slider",
        name="NcLiquidsStainAmount",
        description="Stain Amount:",
        labels={"Fewer Stains", "More Stains"},
        number="F32",
        min=1.0,
        max=60.0,
        section="liquid_collision",
        key="model_stain_rate",
        fallback=24.0
    },
    {
        type="header",
        name="NcLiquidsContactDropletHeader",
        description="Contact Droplets:",
        icon=201
    },
    {
        type="spin",
        name="NcLiquidsContactConnectDroplets",
        description="Connect Contact Droplets:",
        labels={"No", "Yes"},
        values={"OFF", "ON"},
        section="liquid_contact_droplets",
        key="connect_droplets",
        fallback=false
    },
    {
        type="slider",
        name="NcLiquidsContactDropletConnectionDistance",
        description="Connection between linked contacts:",
        labels={"Short", "Long"},
        number="F32",
        min=0.01,
        max=0.10,
        section="liquid_contact_droplets",
        key="connection_distance",
        fallback=0.03
    },
    {
        type="slider",
        name="NcLiquidsContactDropletConnectionThickness",
        description="Connection width between droplets:",
        labels={"Thin", "Wide"},
        number="F32",
        min=0.1,
        max=1.0,
        section="liquid_contact_droplets",
        key="connection_thickness",
        fallback=0.55
    },
    {
        type="slider",
        name="NcLiquidsContactDropletSize",
        description="Size:",
        labels={"Small", "Large"},
        number="F32",
        min=0.001,
        max=0.006,
        section="liquid_contact_droplets",
        key="size",
        fallback=0.005
    },
    {
        type="slider",
        name="NcLiquidsContactDropletOpacity",
        description="Opacity:",
        labels={"Transparent", "Opaque"},
        number="F32",
        min=0.0,
        max=1.0,
        section="liquid_contact_droplets",
        key="opacity",
        fallback=0.66
    },
    {
        type="header",
        name="NcLiquidsEmitterPositionHeader",
        description="Liquid Emitter Position:",
        icon=201
    },
    {
        type="slider",
        name="NcLiquidsEmitterPositionX",
        description="Emitter Position X:",
        labels={"Left", "Right"},
        number="F32",
        min=-1.0,
        max=1.0,
        section="liquid_emitter_position",
        key="position_x",
        fallback=0.0
    },
    {
        type="slider",
        name="NcLiquidsEmitterPositionY",
        description="Emitter Position Y:",
        labels={"Down", "Up"},
        number="F32",
        min=-1.0,
        max=1.0,
        section="liquid_emitter_position",
        key="position_y",
        fallback=0.0
    },
    {
        type="slider",
        name="NcLiquidsEmitterPositionZ",
        description="Emitter Position Z:",
        labels={"Back", "Front"},
        number="F32",
        min=-1.0,
        max=1.0,
        section="liquid_emitter_position",
        key="position_z",
        fallback=0.0
    },
    {
        type="spin",
        name="NcLiquidsTesticularRetractionEnabled",
        description="Testicular Retraction:",
        icon=6826,
        labels={"Off", "On"},
        values={"OFF", "ON"},
        section="testicular_retraction",
        key="enabled",
        fallback=true,
        physx=true
    },
    {
        type="easing",
        name="NcLiquidsTesticularRetractionEasingRetraction",
        description="Retraction Easing:",
        section="testicular_retraction",
        key="retraction_easing",
        fallback="0, 2.0, 0.33, 0.75",
        natural="0, 2.0, 0.33, 0.75",
        physx=true
    },
    {
        type="easing",
        name="NcLiquidsTesticularRetractionEasingRelease",
        description="Release Easing:",
        section="testicular_retraction",
        key="release_easing",
        fallback="0.1, 0.80, 0.40, 1.10",
        natural="0.1, 0.80, 0.40, 1.10",
        physx=true
    },
    {
        type="slider",
        name="NcLiquidsTesticularRetractionStrength",
        description="Retraction Strength:",
        labels={"Weak", "Strong"},
        number="F32",
        min=0.1,
        max=1.0,
        section="testicular_retraction",
        key="target_weight",
        fallback=0.35,
        physx=true
    },
    {
        type="slider",
        name="NcLiquidsTesticularRetractionTimeRetraction",
        description="Retraction Time:",
        labels={"0 seconds", "5 seconds"},
        number="F32",
        min=0.0,
        max=5.0,
        section="testicular_retraction",
        key="retraction_time",
        fallback=0.12,
        physx=true
    },
    {
        type="slider",
        name="NcLiquidsTesticularRetractionTimeHold",
        description="Retraction Hold Time:",
        labels={"0 seconds", "5 seconds"},
        number="F32",
        min=0.0,
        max=5.0,
        section="testicular_retraction",
        key="hold_time",
        fallback=0.50,
        physx=true
    },
    {
        type="slider",
        name="NcLiquidsTesticularRetractionTimeRelease",
        description="Retraction Release Time:",
        labels={"0 seconds", "5 seconds"},
        number="F32",
        min=0.0,
        max=5.0,
        section="testicular_retraction",
        key="release_time",
        fallback=0.50,
        physx=true
    },
    {
        type="preset",
        name="NcLiquidsEjaculationSoundTarget",
        description="Ejaculation Audio Preset:",
        icon=3906,
        labels=ejaculationPresetLabels,
        values=ejaculationPresetValues,
        section="ejaculation_sound",
        key="preset",
        fallback="false",
    },
    {
        type="slider",
        name="NcLiquidsEjaculationSoundVolume",
        description="Ejaculation Audio Volume:",
        labels={"Quiet", "Loud"},
        number="F32",
        min=-10,
        max=10,
        section="ejaculation_sound",
        key="volume",
        fallback=0,
    },
    {
        type="slider",
        name="NcLiquidsEjaculationSoundDelay",
        description="Ejaculation Audio Delay:",
        labels={"Instant", "Delayed"},
        number="F32",
        min=0,
        max=500,
        section="ejaculation_sound",
        key="delay",
        fallback=0,
    },
}

-- Starts the generated BS with the Liquids category definition.
local output = {
    "var :Cat_Liquids I32(8);",
    "",
    "CustomCategory :Category_Liquids . {",
    "\t.CategoryID :Cat_Liquids;",
    "\t.CategoryIconID I32(201);",
    "\t.CategoryDescription \"Liquids\";",
    "\t.CategoryToolTip \"Configure liquid emission, physics, visuals, and retraction.\";",
    "};",
    ""
}

-- Appends one line to the generated BS output.
local function emit(line)
    output[#output + 1] = line
end

-- Defines the first ParamID; every following control receives the next ID.
local paramIDStart = 200

-- Generates each CustomParameter in its configured order.
for controlIndex, control in ipairs(controls) do
    local paramID = paramIDStart + controlIndex - 1

    -- Opens the current CustomParameter definition.
    emit("CustomParameter :Parameter_" .. control.name .. " . {")
    
    -- Starts PhysX-dependent controls disabled; the Liquids DLL enables them when PhysX is loaded.
    if control.physx then emit("\t.Enable False;") end
    
    -- Adds the optional control icon.
    if control.icon then emit("\t.IconID I32(" .. control.icon .. ");") end
    
    -- Adds the control identity, description, and category.
    emit("\t.ParamID I32(" .. paramID .. ");")
    emit("\t.ParamName " .. quote(control.name) .. ";")
    emit("\t.ParamDescription " .. quote(control.description) .. ";")
    emit("\t.CategoryID :Cat_Liquids;")

    -- Generates a visual section header.
    if control.type == "header" then
        emit("\t.ParamType CustomParamType .Presets;")
    
    -- Generates a numeric slider using its current INI value.
    elseif control.type == "slider" then
        local current = ini_number(control.section, control.key, control.fallback)
        
        emit("\t.ParamType CustomParamType .Slider;")
        emit("\t.SliderLabelArray [ " .. string_array(control.labels) .. " ];")
        emit("\t.SliderDefault " .. typed_number(control.number, current) .. ";")
        emit("\t.SliderRange ( " .. typed_number(control.number, control.min) ..
             " , " .. typed_number(control.number, control.max) .. " );")
    
    -- Generates a spin-box or easing selector.
    else
        local labels = control.labels
        local values = control.values
        local current

        -- Builds the easing choices around the saved easing value.
        if control.type == "easing" then
            current = ini_value(control.section, control.key, control.fallback)
            labels = { "Linear", "Smooth", current == control.natural and "Natural" or "Custom" }
            values = { "linear", "smooth", control.natural }
            
            -- Shows a saved custom curve as the third easing choice.
            if current ~= "linear" and current ~= "smooth" then
                values[3] = current
            end

        -- Uses the selected preset folder name directly as the saved value.
        elseif control.type == "preset" then
            current = ini_value(control.section, control.key,
                                control.fallback)
            local available = false
            for _, value in ipairs(values) do
                if value:lower() == current:lower() then
                    current = value
                    available = true
                    break
                end
            end
            if not available then current = "false" end
        
        -- Converts a regular switch value to ON or OFF.
        else
            current = ini_bool(control.section, control.key, control.fallback) and "ON" or "OFF"
        end
        
        -- Writes the spin-box choices and selected value.
        emit("\t.ParamType CustomParamType .SpinBox;")
        emit("\t.BoxDescriptionArray String [ " .. string_array(labels) .. " ];")
        emit("\t.BoxStringValueArray String [ " .. string_array(values) .. " ];")
        emit("\t.BoxDefaultString " .. quote(current) .. ";")
    end

    -- Finish the CUstomParam block
    emit("};")
    emit("")
end

-- Publishes the generated virtual CcConfigEditorLiquids.bs resource.
add(table.concat(output, "\n"))

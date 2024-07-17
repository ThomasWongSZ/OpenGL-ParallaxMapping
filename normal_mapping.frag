
#version 330 core
in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir);
vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir);
void main(void)
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    //vec2 texCoords = ParallaxMapping(TexCoords, viewDir);
    //vec2 texCoords = SteepParallaxMapping(TexCoords, viewDir);
    vec2 texCoords = ParallaxOcclusionMapping(TexCoords, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    vec3 normal = texture(normalMap, texCoords).rgb;    // obtain normal from normal map in range [0,1]
    normal = normalize(normal * 2.0 - 1.0);             // transform normal vector to range [-1,1]
    //vec3 normal = vec3(0.0, 0.0, 1.0);

    /* blinn-phong */
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

/**
  * Traditional Parallax mapping
  * It will be weird if you look at the surface from an angle cause it's just a crude approximation of displacement mapping.
  *
  * note that the positive z direction equals the surface's normal vector in tangent space
  */
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
    float height = texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}

/**
  * Steep Parallax mapping
  */
vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir){

    // By taking less samples when looking straight at a surface and more samples
    // when looking at an angle we only sample the necessary amount:
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    // number of depth layers
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue){
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir){
    // By taking less samples when looking straight at a surface and more samples
    // when looking at an angle we only sample the necessary amount:
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    // number of depth layers
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue){
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

























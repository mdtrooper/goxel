/* Goxel 3D voxels editor
 *
 * copyright (c) 2018 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file is autogenerated by tools/create_assets.py */

{.path = "data/shaders/background.glsl", .size = 570, .data =
    "varying lowp vec4 v_color;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp vec3 a_pos;\n"
    "attribute lowp  vec4 a_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(a_pos, 1.0);\n"
    "    v_color = a_color;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/mesh.glsl", .size = 4226, .data =
    "/*\n"
    " * I followed those name conventions.  All the vectors are expressed in eye\n"
    " * coordinates.\n"
    " *\n"
    " *         reflection         light source\n"
    " *\n"
    " *               r              s\n"
    " *                 ^         ^\n"
    " *                  \\   n   /\n"
    " *  eye              \\  ^  /\n"
    " *     v  <....       \\ | /\n"
    " *              -----__\\|/\n"
    " *                  ----+----\n"
    " *\n"
    " *\n"
    " */\n"
    "\n"
    "uniform highp mat4  u_model;\n"
    "uniform highp mat4  u_view;\n"
    "uniform highp mat4  u_proj;\n"
    "uniform highp mat4  u_shadow_mvp;\n"
    "uniform lowp  float u_pos_scale;\n"
    "\n"
    "// Light parameters\n"
    "uniform lowp    vec3  u_l_dir;\n"
    "uniform lowp    float u_l_int;\n"
    "\n"
    "// Material parameters\n"
    "uniform lowp float u_m_amb; // Ambient light coef.\n"
    "uniform lowp float u_m_dif; // Diffuse light coef.\n"
    "uniform lowp float u_m_spe; // Specular light coef.\n"
    "uniform lowp float u_m_shi; // Specular light shininess.\n"
    "uniform lowp float u_m_smo; // Smoothness.\n"
    "\n"
    "uniform mediump sampler2D u_bshadow_tex;\n"
    "uniform mediump sampler2D u_bump_tex;\n"
    "uniform mediump float     u_bshadow;\n"
    "uniform mediump sampler2D u_shadow_tex;\n"
    "uniform mediump float     u_shadow_k;\n"
    "\n"
    "varying highp   vec3 v_pos;\n"
    "varying lowp    vec4 v_color;\n"
    "varying mediump vec2 v_bshadow_uv;\n"
    "varying mediump vec2 v_uv;\n"
    "varying mediump vec2 v_bump_uv;\n"
    "varying mediump vec3 v_normal;\n"
    "varying mediump vec4 v_shadow_coord;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3 a_pos;\n"
    "attribute mediump vec3 a_normal;\n"
    "attribute lowp    vec4 a_color;\n"
    "attribute mediump vec2 a_bshadow_uv;\n"
    "attribute mediump vec2 a_bump_uv;   // bump tex base coordinates [0,255]\n"
    "attribute mediump vec2 a_uv;        // uv coordinates [0,1]\n"
    "\n"
    "void main()\n"
    "{\n"
    "    v_normal = a_normal;\n"
    "    v_color = a_color;\n"
    "    v_bshadow_uv = (a_bshadow_uv + 0.5) / (16.0 * VOXEL_TEXTURE_SIZE);\n"
    "    v_pos = a_pos * u_pos_scale;\n"
    "    v_uv = a_uv;\n"
    "    v_bump_uv = a_bump_uv;\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(v_pos, 1.0);\n"
    "    v_shadow_coord = (u_shadow_mvp * u_model * vec4(v_pos, 1.0));\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "mediump vec2 uv, bump_uv;\n"
    "mediump vec3 n, s, r, v, bump;\n"
    "mediump float s_dot_n;\n"
    "lowp float l_amb, l_dif, l_spe;\n"
    "lowp float bshadow;\n"
    "lowp float visibility;\n"
    "lowp vec2 PS[4]; // Poisson offsets used for the shadow map.\n"
    "int i;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // clamp uv so to prevent overflow with multismapling.\n"
    "    uv = clamp(v_uv, 0.0, 1.0);\n"
    "    s = u_l_dir;\n"
    "    n = normalize((u_view * u_model * vec4(v_normal, 0.0)).xyz);\n"
    "    bump_uv = (v_bump_uv + 0.5 + uv * 15.0) / 256.0;\n"
    "    bump = texture2D(u_bump_tex, bump_uv).xyz - 0.5;\n"
    "    bump = normalize((u_view * u_model * vec4(bump, 0.0)).xyz);\n"
    "    n = mix(bump, n, u_m_smo);\n"
    "    s_dot_n = dot(s, n);\n"
    "    l_dif = u_m_dif * max(0.0, s_dot_n);\n"
    "    l_amb = u_m_amb;\n"
    "\n"
    "    // Specular light.\n"
    "    v = normalize(-(u_view * u_model * vec4(v_pos, 1.0)).xyz);\n"
    "    r = reflect(-s, n);\n"
    "    l_spe = u_m_spe * pow(max(dot(r, v), 0.0), u_m_shi);\n"
    "    l_spe = s_dot_n > 0.0 ? l_spe : 0.0;\n"
    "\n"
    "    bshadow = texture2D(u_bshadow_tex, v_bshadow_uv).r;\n"
    "    bshadow = sqrt(bshadow);\n"
    "    bshadow = mix(1.0, bshadow, u_bshadow);\n"
    "    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "    gl_FragColor.rgb += (l_dif + l_amb) * u_l_int * v_color.rgb;\n"
    "    gl_FragColor.rgb += l_spe * u_l_int * vec3(1.0);\n"
    "    gl_FragColor.rgb *= bshadow;\n"
    "\n"
    "    // Shadow map.\n"
    "    #ifdef SHADOW\n"
    "    visibility = 1.0;\n"
    "    mediump vec4 shadow_coord = v_shadow_coord / v_shadow_coord.w;\n"
    "    lowp float bias = 0.005 * tan(acos(clamp(s_dot_n, 0.0, 1.0)));\n"
    "    bias = clamp(bias, 0.0, 0.015);\n"
    "    shadow_coord.z -= bias;\n"
    "    PS[0] = vec2(-0.94201624, -0.39906216) / 1024.0;\n"
    "    PS[1] = vec2(+0.94558609, -0.76890725) / 1024.0;\n"
    "    PS[2] = vec2(-0.09418410, -0.92938870) / 1024.0;\n"
    "    PS[3] = vec2(+0.34495938, +0.29387760) / 1024.0;\n"
    "    for (i = 0; i < 4; i++)\n"
    "        if (texture2D(u_shadow_tex, v_shadow_coord.xy +\n"
    "           PS[i]).z < shadow_coord.z) visibility -= 0.2;\n"
    "    if (s_dot_n <= 0.0) visibility = 0.5;\n"
    "    gl_FragColor.rgb *= mix(1.0, visibility, u_shadow_k);\n"
    "    #endif\n"
    "\n"
    "}\n"
    "\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/model3d.glsl", .size = 2766, .data =
    "#if defined(GL_ES) && defined(FRAGMENT_SHADER)\n"
    "#extension GL_OES_standard_derivatives : enable\n"
    "#endif\n"
    "\n"
    "uniform highp   mat4  u_model;\n"
    "uniform highp   mat4  u_view;\n"
    "uniform highp   mat4  u_proj;\n"
    "uniform highp   mat4  u_clip;\n"
    "uniform lowp    vec4  u_color;\n"
    "uniform mediump vec2  u_uv_scale;\n"
    "uniform lowp    float u_grid_alpha;\n"
    "uniform mediump vec3  u_l_dir;\n"
    "uniform mediump float u_l_diff;\n"
    "uniform mediump float u_l_emit;\n"
    "\n"
    "uniform mediump sampler2D u_tex;\n"
    "uniform mediump float     u_strip;\n"
    "uniform mediump float     u_time;\n"
    "\n"
    "varying mediump vec3 v_normal;\n"
    "varying highp   vec3 v_pos;\n"
    "varying lowp    vec4 v_color;\n"
    "varying mediump vec2 v_uv;\n"
    "varying mediump vec4 v_clip_pos;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3  a_pos;\n"
    "attribute lowp    vec4  a_color;\n"
    "attribute mediump vec3  a_normal;\n"
    "attribute mediump vec2  a_uv;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    lowp vec4 col = u_color * a_color;\n"
    "    highp vec3 pos = (u_view * u_model * vec4(a_pos, 1.0)).xyz;\n"
    "    if (u_clip[3][3] > 0.0)\n"
    "        v_clip_pos = u_clip * u_model * vec4(a_pos, 1.0);\n"
    "    gl_Position = u_proj * vec4(pos, 1.0);\n"
    "    mediump float diff = max(0.0, dot(u_l_dir, a_normal));\n"
    "    col.rgb *= (u_l_emit + u_l_diff * diff);\n"
    "    v_color = col;\n"
    "    v_uv = a_uv * u_uv_scale;\n"
    "    v_pos = (u_model * vec4(a_pos, 1.0)).xyz;\n"
    "    v_normal = a_normal;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color * texture2D(u_tex, v_uv);\n"
    "    if (u_strip > 0.0) {\n"
    "       mediump float p = gl_FragCoord.x + gl_FragCoord.y + u_time * 4.0;\n"
    "       if (mod(p, 8.0) < 4.0) gl_FragColor.rgb *= 0.5;\n"
    "    }\n"
    "    if (u_clip[3][3] > 0.0) {\n"
    "        if (    v_clip_pos[0] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[1] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[2] < -v_clip_pos[3] ||\n"
    "                v_clip_pos[0] > +v_clip_pos[3] ||\n"
    "                v_clip_pos[1] > +v_clip_pos[3] ||\n"
    "                v_clip_pos[2] > +v_clip_pos[3]) discard;\n"
    "    }\n"
    "\n"
    "    // Grid effect.\n"
    "#if !defined(GL_ES) || defined(GL_OES_standard_derivatives)\n"
    "    if (u_grid_alpha > 0.0) {\n"
    "        mediump vec2 c;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).x) > 0.5) c = v_pos.yz;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).y) > 0.5) c = v_pos.zx;\n"
    "        if (abs((u_model * vec4(v_normal, 0.0)).z) > 0.5) c = v_pos.xy;\n"
    "        mediump vec2 grid = abs(fract(c - 0.5) - 0.5) / fwidth(c);\n"
    "        mediump float line = min(grid.x, grid.y);\n"
    "        gl_FragColor.rgb *= mix(1.0 - u_grid_alpha, 1.0, min(line, 1.0));\n"
    "    }\n"
    "#endif\n"
    "\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/pos_data.glsl", .size = 790, .data =
    "varying lowp  vec2 v_pos_data;\n"
    "uniform highp mat4 u_model;\n"
    "uniform highp mat4 u_view;\n"
    "uniform highp mat4 u_proj;\n"
    "uniform lowp  vec2 u_block_id;\n"
    "\n"
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp vec3 a_pos;\n"
    "attribute lowp  vec2 a_pos_data;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    highp vec3 pos = a_pos;\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);\n"
    "    v_pos_data = a_pos_data;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor.rg = u_block_id;\n"
    "    gl_FragColor.ba = v_pos_data;\n"
    "}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},
{.path = "data/shaders/shadow_map.glsl", .size = 639, .data =
    "#ifdef VERTEX_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "attribute highp   vec3  a_pos;\n"
    "uniform   highp   mat4  u_model;\n"
    "uniform   highp   mat4  u_view;\n"
    "uniform   highp   mat4  u_proj;\n"
    "uniform   mediump float u_pos_scale;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_proj * u_view * u_model * vec4(a_pos * u_pos_scale, 1.0);\n"
    "}\n"
    "\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    "\n"
    "#ifdef FRAGMENT_SHADER\n"
    "\n"
    "/************************************************************************/\n"
    "void main() {}\n"
    "/************************************************************************/\n"
    "\n"
    "#endif\n"
    ""
},




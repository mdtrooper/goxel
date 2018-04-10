/* Goxel 3D voxels editor
 *
 * copyright (c) 2017 Guillaume Chereau <guillaume@noctua-software.com>
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

#include "goxel.h"


typedef struct {
    tool_t tool;
    float  box[4][4];
    int    snap_face;
    struct {
        gesture3d_t move;
    } gestures;
} tool_move_t;

static void do_move(layer_t *layer, const float mat[4][4])
{
    float m[4][4];

    mat4_set_identity(m);
    // Change referential to the mesh origin.
    // XXX: maybe this should be done in mesh_move directy??
    mat4_itranslate(m, -0.5, -0.5, -0.5);
    mat4_imul(m, mat);
    mat4_itranslate(m, +0.5, +0.5, +0.5);

    if (layer->base_id || layer->image) {
        mat4_mul(mat, layer->mat, layer->mat);
        layer->base_mesh_key = 0;
    } else {
        mesh_move(layer->mesh, m);
        if (!box_is_null(layer->box)) {
            mat4_mul(mat, layer->box, layer->box);
            box_get_bbox(layer->box, layer->box);
        }
    }
    goxel_update_meshes(goxel, -1);
}

// Get the face index from the normal.
// XXX: used in a few other places!
static int get_face(const float n[3])
{
    int f;
    const int *n2;
    for (f = 0; f < 6; f++) {
        n2 = FACES_NORMALS[f];
        if (vec3_dot(n, VEC(n2[0], n2[1], n2[2])) > 0.5)
            return f;
    }
    return -1;
}

static int on_move(gesture3d_t *gest, void *user)
{
    float face_plane[4][4];
    cursor_t *curs = gest->cursor;
    tool_move_t *tool = user;
    float n[3], pos[3], d[3], ofs[3], v[3];
    layer_t *layer = goxel->image->active_layer;
    float mat[4][4];

    if (box_is_null(tool->box)) return GESTURE_FAILED;

    if (gest->type == GESTURE_HOVER) {
        goxel_set_help_text(goxel, "Drag to move face");
        if (curs->snaped != SNAP_LAYER_OUT) return GESTURE_FAILED;
        tool->snap_face = get_face(curs->normal);
        curs->snap_offset = 0;
        curs->snap_mask &= ~SNAP_ROUNDED;
        mat4_mul(tool->box, FACES_MATS[tool->snap_face], face_plane);
        render_img(&goxel->rend, NULL, face_plane, EFFECT_NO_SHADING);
        if (curs->flags & CURSOR_PRESSED) {
            gest->type = GESTURE_DRAG;
            vec3_normalize(face_plane[0], v);
            plane_from_vectors(goxel->tool_plane, curs->pos, curs->normal, v);
            image_history_push(goxel->image);
        }
        return 0;
    }
    if (gest->type == GESTURE_DRAG) {
        goxel_set_help_text(goxel, "Drag to move face");
        curs->snap_offset = 0;
        curs->snap_mask &= ~SNAP_ROUNDED;
        mat4_mul(tool->box, FACES_MATS[tool->snap_face], face_plane);

        vec3_normalize(face_plane[2], n);
        vec3_sub(curs->pos, goxel->tool_plane[3], v);
        vec3_project(v, n, v);
        vec3_add(goxel->tool_plane[3], v, pos);
        pos[0] = round(pos[0]);
        pos[1] = round(pos[1]);
        pos[2] = round(pos[2]);
        vec3_add(tool->box[3], face_plane[2], d);
        vec3_sub(pos, d, ofs);
        vec3_project(ofs, n, ofs);

        mat4_set_identity(mat);
        mat4_itranslate(mat, ofs[0], ofs[1], ofs[2]);
        do_move(layer, mat);

        if (gest->state == GESTURE_END) {
            gest->type = GESTURE_HOVER;
            mat4_copy(plane_null, goxel->tool_plane);
        }
        return 0;
    }
    return 0;
}

static int iter(tool_t *tool, const float viewport[4])
{
    tool_move_t *move = (tool_move_t*)tool;
    layer_t *layer = goxel->image->active_layer;
    cursor_t *curs = &goxel->cursor;
    curs->snap_mask = SNAP_LAYER_OUT;

    if (!move->gestures.move.type) {
        move->gestures.move = (gesture3d_t) {
            .type = GESTURE_HOVER,
            .callback = on_move,
        };
    }
    gesture3d(&move->gestures.move, curs, move);
    mesh_get_box(layer->mesh, true, move->box);
    render_box(&goxel->rend, move->box, NULL,
               EFFECT_STRIP | EFFECT_WIREFRAME);
    return 0;
}

static int gui(tool_t *tool)
{
    layer_t *layer;
    float mat[4][4] = MAT4_IDENTITY;
    int i;
    double v;

    layer = goxel->image->active_layer;
    gui_group_begin(NULL);
    i = 0;
    if (gui_input_int("Move X", &i, 0, 0))
        mat4_itranslate(mat, i, 0, 0);
    i = 0;
    if (gui_input_int("Move Y", &i, 0, 0))
        mat4_itranslate(mat, 0, i, 0);
    i = 0;
    if (gui_input_int("Move Z", &i, 0, 0))
        mat4_itranslate(mat, 0, 0, i);
    gui_group_end();
    gui_group_begin(NULL);
    i = 0;
    if (gui_input_int("Rot X", &i, 0, 0))
        mat4_irotate(mat, i * M_PI / 2, 1, 0, 0);
    i = 0;
    if (gui_input_int("Rot Y", &i, 0, 0))
        mat4_irotate(mat, i * M_PI / 2, 0, 1, 0);
    i = 0;
    if (gui_input_int("Rot Z", &i, 0, 0))
        mat4_irotate(mat, i * M_PI / 2, 0, 0, 1);
    gui_group_end();
    if (layer->image && gui_input_int("Scale", &i, 0, 0)) {
        v = pow(2, i);
        mat4_iscale(mat, v, v, v);
    }

    gui_group_begin(NULL);
    if (gui_button("flip X", -1, 0)) mat4_iscale(mat, -1,  1,  1);
    if (gui_button("flip Y", -1, 0)) mat4_iscale(mat,  1, -1,  1);
    if (gui_button("flip Z", -1, 0)) mat4_iscale(mat,  1,  1, -1);
    gui_group_end();

    if (memcmp(&mat, &mat4_identity, sizeof(mat))) {
        image_history_push(goxel->image);
        do_move(layer, mat);
    }
    return 0;
}

TOOL_REGISTER(TOOL_MOVE, move, tool_move_t,
              .iter_fn = iter,
              .gui_fn = gui,
              .flags = TOOL_REQUIRE_CAN_MOVE,
              .shortcut = "M",
)

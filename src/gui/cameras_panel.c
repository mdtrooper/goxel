/* Goxel 3D voxels editor
 *
 * copyright (c) 2019 Guillaume Chereau <guillaume@noctua-software.com>
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

void gui_cameras_panel(void)
{
    camera_t *cam;
    int i = 0;
    bool current;
    gui_group_begin(NULL);
    DL_FOREACH(goxel.image->cameras, cam) {
        current = goxel.image->active_camera == cam;
        if (gui_layer_item(i, -1, NULL, &current, cam->name, sizeof(cam->name))) {
            goxel.image->active_camera = cam;
        }
        i++;
    }
    gui_group_end();
    gui_action_button("img_new_camera", NULL, 0, "");
    gui_same_line();
    gui_action_button("img_del_camera", NULL, 0, "");
    gui_same_line();
    gui_action_button("img_move_camera_up", NULL, 0, "");
    gui_same_line();
    gui_action_button("img_move_camera_down", NULL, 0, "");

    if (!goxel.image->cameras) image_add_camera(goxel.image, NULL);


    cam = goxel.image->active_camera;
    gui_input_float("dist", &cam->dist, 10.0, 0, 0, NULL);

    /*
    gui_group_begin("Offset");
    gui_input_float("x", &cam->ofs[0], 1.0, 0, 0, NULL);
    gui_input_float("y", &cam->ofs[1], 1.0, 0, 0, NULL);
    gui_input_float("z", &cam->ofs[2], 1.0, 0, 0, NULL);
    gui_group_end();

    gui_quat("Rotation", cam->rot);
    */
    gui_checkbox("Ortho", &cam->ortho, NULL);

    gui_group_begin("Set");
    gui_action_button("view_left", "left", 0.5, ""); gui_same_line();
    gui_action_button("view_right", "right", 1.0, "");
    gui_action_button("view_front", "front", 0.5, ""); gui_same_line();
    gui_action_button("view_top", "top", 1.0, "");
    gui_action_button("view_default", "default", 1.0, "");
    gui_group_end();
}


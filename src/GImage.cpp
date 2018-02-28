/*
 * This file is part of the VSS-Vision project.
 *
 * This Source Code Form is subject to the terms of the GNU GENERAL PUBLIC LICENSE,
 * v. 3.0. If a copy of the GPL was not distributed with this
 * file, You can obtain one at http://www.gnu.org/licenses/gpl-3.0/.
 */

#include "GImage.h"

GImage::GImage(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::DrawingArea(cobject) {
    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_MOTION_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::POINTER_MOTION_MASK);

    cut_mode = true;
    cut_move = false;
    cut_move_adjust = false;

    cv_image = cv::Mat::zeros(1, 1, CV_64F);
}

GImage::~GImage(){ 
}

bool GImage::on_expose_event(GdkEventExpose* event) {

    try{
        Glib::RefPtr<Gdk::Window> window = get_window();

        //float width  = get_allocation().get_width();
        //float height = get_allocation().get_height();

        cv::resize(cv_image, cv_image, cv::Point(800 , 640), 0, 0, cv::INTER_LINEAR);

        Cairo::RefPtr<Cairo::Context> c = window->create_cairo_context();
        
        Glib::RefPtr<Gdk::Pixbuf> pixbuf =  Gdk::Pixbuf::create_from_data( cv_image.data, Gdk::COLORSPACE_RGB, false, 8, cv_image.cols, cv_image.rows, cv_image.step);
        Gdk::Cairo::set_source_pixbuf(c, pixbuf);
        c->paint();

        if (cut_mode) {
            c->move_to(cut_point_1.x , cut_point_1.y);
            c->line_to(cut_point_2.x , cut_point_1.y);
            c->line_to(cut_point_2.x , cut_point_2.y);
            c->line_to(cut_point_1.x , cut_point_2.y);
            c->close_path();

            std::vector<double> dash = {20.0, 20.0};
            c->set_dash(dash, 1);
            c->set_source_rgba(1, 1, 1, 0.8);
            c->set_line_width(3.0);
            c->stroke();
        }

    } catch(const std::exception& ex) {
        std::cout << "EXCEPTION: " << ex.what() << "in GImage:on_draw " << std::endl;
    }
    
    return true;
}

bool GImage::on_button_press_event (GdkEventButton* event){
    if (event->button == 1 && cut_mode) {
        cut_move_adjust = true;
    }
   
    if (event->button == 3 && cut_mode) {
        cut_point_1 = {int(event->x), int(event->y)};
        cut_point_2 = {int(event->x), int(event->y)};
        cut_move = true;
    } 

    return true;
}

bool GImage::on_button_release_event (GdkEventButton* event){
    if (event->button == 1 && cut_mode) {
        cut_move_adjust = false;
    }

    if (event->button == 3 && cut_mode) {
        cut_move = false;
    }

    return true;
}

bool GImage::on_motion_notify_event (GdkEventMotion* event){
    if (cut_move && cut_mode) {
        cut_point_2 = {int(event->x) , int(event->y)};
        queue_draw();
    
    } else if (cut_move_adjust && cut_mode) {

        int dist1 = abs(int(event->x) - cut_point_1.x);
        int dist2 = abs(int(event->x) - cut_point_2.x);
        int dist3 = abs(int(event->y) - cut_point_2.y);
        int dist4 = abs(int(event->y) - cut_point_1.y);

        if (dist1 < dist2 && dist1 < dist3 && dist1 < dist4){
            cut_point_1.x = int(event->x);
        } else if (dist2 < dist1 && dist2 < dist3 && dist2 < dist4){
            cut_point_2.x = int(event->x);
        } else if (dist3 < dist1 && dist3 < dist2 && dist3 < dist4){
            cut_point_2.y = int(event->y);
        } else if (dist4 < dist1 && dist4 < dist2 && dist4 < dist3){
            cut_point_1.y = int(event->y);
        }
        
        queue_draw();
    }
    return true;
}

void GImage::set_image(cv::Mat _cv_image){
    cv::cvtColor(_cv_image, cv_image, cv::COLOR_BGR2RGB);
    queue_draw();
}

void GImage::set_cut_mode(bool _cut_mode){
    cut_mode = _cut_mode;
}
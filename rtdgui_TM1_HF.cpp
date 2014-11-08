#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Input.H> // necessary for bug in mingw32?
#include <FL/Fl_Box.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Input.H>
#include <FL/fl_draw.H>
#include <FL/forms.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Radio_Button.H>
#include <string.h>
#include <time.h>
#include "Cartesian.H"
#include "Cartesian.cpp"
#include "radio.h"
#include <FLU/Flu_Dual_Slider.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "imgsav.h"

using namespace std;

//uncomment this line for big monitor
int sx = 1200, sy = 872, plot_max = 60, plot_min = 0;
//uncomment this line for square monitor
//int sx = 1024, sy = 768, plot_max = 60, plot_min = 0;
int gray_max = 60, gray_min = 0;
//static char site_str[50];
char timestamp_string[100], config_filename[200], tmp_dir[100], prefix[100];
char instring[100];
static struct tm *gmt;
time_t t;

Fl_Output *cur_time = (Fl_Output *) 0;

Fl_Button *savspec1_button, *savspec2_button;
int savspec1 = 1;

Fl_Round_Button *show_ch1,
	*o1gray1, *o1gray2, *o1gray3, *o1gray4,
	*show_time_series;
Fl_Double_Window *win;
Flu_Dual_Slider *ds, *ds2;

FILE *in, *current;
char fname[200], n_fname[200], p_string[200], data_fname[200];

//additions by Spencer Hatch for config file, displays
char disp_name[200];
long int freq_min = 0, freq_max = 1000, freq_step=40;

Fl_Shared_Image *img1, *img2;
unsigned long t_count, c_count;

Ca_Canvas *canvas, *spec1;
Ca_X_Axis *freq1;
Ca_Y_Axis *counts, *freq2;

void plot_level_callback(Fl_Widget*, void*) {
	static char outstring[50];

	plot_max = (int) (ds->high_value() * 100. + 0.5);
	plot_min = (int) (ds->low_value() * 100. + 0.5);
	sprintf(outstring, "Plot Range %d:%d", plot_min, plot_max);
	ds->label(outstring);
	counts->maximum(plot_max);
	counts->minimum(plot_min);
}

void grayscale_level_callback(Fl_Widget*, void*) {
	static char outstring[50];

	gray_max = (int) (ds2->high_value() * 100. + 0.5);
	gray_min = (int) (ds2->low_value() * 100. + 0.5);
	sprintf(outstring, "Grayscale Range %d:%d", gray_min, gray_max);
	ds2->label(outstring);
}

void savspec_callback(Fl_Widget *, void *v) {
  int *savspec;
  char filename[100];

  savspec = (int *)v;
  time(&t);
  gmt = gmtime(&t);
  sprintf(filename, "rtdgui_spec%i-%04i%02i%02i-%02i%02i%02i.png",*savspec,gmt->tm_year+1900, gmt->tm_mon+1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);

  if( *savspec == 1 ) {
    if( (img1) && ( img1->count() > 0 ) ) {
      printf("Bout to save Ch 1 spec to %s\n",filename);
      ImageHandler::save(img1,filename);
    } 
    else {
      printf("Couldn't save spectrum %i! No data?\n",*savspec);
    }
  }   
  
  if( *savspec == 2 ) {
    if( (img2) && ( img2->count() > 0 ) ){
      printf("Bout to save Ch 2 spec to %s\n",filename);
      ImageHandler::save(img2,filename);
    } 
    else {
      printf("Couldn't save spectrum %i! No data?\n",*savspec);
    }
  }

}

void WinQuit_CB(Fl_Widget*, void*) {
	FILE *out;
	sprintf(instring, "%s/%s_levels.grayscale", tmp_dir, prefix);
	out = fopen(instring, "w");
	fprintf(out, "%d %d", gray_min, gray_max);
	fclose(out);
	sprintf(instring, "%s/%s_levels.plot", tmp_dir, prefix);
	out = fopen(instring, "w");
	fprintf(out, "%d %d", plot_min, plot_max);
	fclose(out);
	sprintf(instring, "%s/%s_hf2_display_running", tmp_dir, prefix);
	remove(instring);
	exit(0);
}

int read_level_files() {
	FILE * in;

	sprintf(instring, "%s/%s_levels.grayscale", tmp_dir, prefix);
	in = fopen(instring, "r");
	if (in != NULL) {
		fscanf(in, "%d %d", &gray_min, &gray_max);
		fclose(in);
	}
	sprintf(instring, "%s/%s_levels.plot", tmp_dir, prefix);
	in = fopen(instring, "r");
	if (in != NULL) {
		fscanf(in, "%d %d", &plot_min, &plot_max);
		fclose(in);
	}
	
	return (1);
}

int read_input_file() {
	char line[80];
	FILE * in;

	fprintf(stderr, "\nReading Input File ... ");

	in = fopen(config_filename, "r");
	if (in == NULL) {
		fprintf(stderr, "Error! hf2_display could not open input file: %s\n",
				config_filename);
		exit(-1);
	}
	while (fgets(line, sizeof(line), in) != 0) {
		if (strncmp(line, "TMP", 3) == 0) {
			fgets(tmp_dir, sizeof(tmp_dir), in);
			tmp_dir[strlen(tmp_dir) - 1] = 0;
			fprintf(stderr, "\nFound TMP_DIR=%s\n", tmp_dir);
		}
		if (strncmp(line, "DISP_NAME", 6) == 0) {
			fgets(disp_name, sizeof(disp_name), in);
			disp_name[strlen(disp_name) - 1] = 0;
			fprintf(stderr, "\nFound DISP_NAME=%s\n", disp_name);
		} 
		if (strncmp(line, "MAX_FREQ", 6) == 0) {
			fgets(line, sizeof(line), in);
			freq_max=atoi(line);
			fprintf(stderr, "\nFound MAX_FREQ=%li\n", freq_max);
		}
		if (strncmp(line, "MIN_FREQ", 6) == 0) {
			fgets(line, sizeof(line), in);
			freq_min=atoi(line);
			fprintf(stderr, "\nFound MIN_FREQ=%li\n", freq_min);
		}
		if (strncmp(line, "STEP_FREQ", 6) == 0) {
			fgets(line, sizeof(line), in);
			freq_step=atoi(line);
			fprintf(stderr, "\nFound STEP_FREQ=%li\n", freq_step);
		}
		if (strncmp(line, "MAX_GRAY", 6) == 0) {
			fgets(line, sizeof(line), in);
			gray_max=atoi(line);
			fprintf(stderr, "\nFound MAX_GRAY=%i\n", gray_max);
		}
		if (strncmp(line, "MIN_GRAY", 6) == 0) {
			fgets(line, sizeof(line), in);
			gray_max=atoi(line);
			fprintf(stderr, "\nFound MIN_GRAY=%i\n", gray_min);
		}
	}
	fclose(in);

	return (1);
}

void load_data(const char *n) {
	FILE * in;
	float y1[512], y2[512], y3[512], y4[512], x[512];
	int i, s1;
	//	static Ca_PolyLine *P_P = 0;

	in = fopen(data_fname, "r");
	if (in != NULL) {
		canvas->current();
		canvas->clear();
		counts->current();
		s1 = ((Fl_Button *) show_ch1)->value();
		for (i = 0; i < 512; i++) {
			fscanf(in, "%f %f %f %f %f", x + i, y1 + i, y2 + i, y3 + i, y4 + i);
			if (s1)
				new Ca_Point(x[i], y1[i], FL_YELLOW, CA_DIAMOND | CA_BORDER, 2);
		}
		fclose(in);
		canvas->redraw();
	} else {
		new Ca_Text(800, 1, "No Data File Found!");
	}
}

void load_file(const char *n) {
  //	float scale = 1.0, ratio = 1.0;
	if (img1) {
		img1->release();
		img1 = 0L;
	}
	img1 = Fl_Shared_Image::get(n);
	if (!img1) {
		spec1->label(""); // show an empty document
		spec1->labelsize(64);
		spec1->labelcolor(FL_LIGHT2);
		spec1->image(0);
		spec1->redraw();
		return;
	}
	spec1->label(0);
	spec1->labelsize(1);
	spec1->align(FL_ALIGN_IMAGE_OVER_TEXT);
	spec1->labelcolor(FL_FOREGROUND_COLOR);
	spec1->image(img1);
	spec1->redraw();

}

//--------------------------------------------
void close_cb(Fl_Widget* o, void*) {
	exit(0);
}

//--------------------------------------------
void callback(void*) {
  FILE *out;
  time(&t);
  gmt = gmtime(&t);
  sprintf(timestamp_string, "%s", asctime(gmt));
  timestamp_string[strlen(timestamp_string) - 1] = 0;
  sprintf(timestamp_string, "%s UT", timestamp_string);
  cur_time->value(timestamp_string);
  if (o1gray1->value() == 1)
    sprintf(fname, "%s/%s.image1", tmp_dir, prefix);
  if (o1gray2->value() == 1)
    sprintf(fname, "%s/%s.image2", tmp_dir, prefix);
  if (o1gray3->value() == 1)
    sprintf(fname, "%s/%s.image3", tmp_dir, prefix);
  if (o1gray4->value() == 1)
    sprintf(fname, "%s/%s.image4", tmp_dir, prefix);
  load_file(fname);
  load_data(data_fname);
  sprintf(instring, "%s/%s_levels.grayscale", tmp_dir, prefix);
  out = fopen(instring, "w");
  fprintf(out, "%d %d", gray_min, gray_max);
  fclose(out);
  sprintf(instring, "%s/%s_levels.plot", tmp_dir, prefix);
  out = fopen(instring, "w");
  fprintf(out, "%d %d", plot_min, plot_max);
  fclose(out);
  Fl::repeat_timeout(1.0, callback);
}

//--------------------------------------------
int main(int argc, char **argv) {

	unsigned long i = 0;
	char outstring1[50];
	char outstring2[50];
	FILE *out;

	sprintf(disp_name,"HF2 Display Receiver"); //default display name


	/* read location for the config file if given */
	if (argc == 2 || argc == 3 ) {
	  sprintf(config_filename, "%s", argv[1]);
	  if ( argc == 3 ) sprintf(prefix, "%s", argv[2]);
	  else sprintf(prefix, "test");
	}
	else {
	  sprintf(config_filename,
		  "./hf2_config.input");
	}
	read_input_file();
	sprintf(data_fname, "%s/%s.data", tmp_dir, prefix);

	nice(10);

	sprintf(instring, "%s/%s_hf2_display_running", tmp_dir, prefix);
	out = fopen(instring, "r");
	if (out != NULL) {
		fprintf(stderr, "\nhf2_display found a lock file ... ");
		fscanf(out, "%lu", &i);
		fprintf(stderr, "\n  PID: %lu", i);
		fclose(out);
		sprintf(outstring1, "/proc/%lu/cmdline", i);
		out = fopen(outstring1, "r");
		if (out != NULL) {
			fscanf(out, "%s", outstring1);
			sprintf(instring, "hf2_display");
			if (strstr(outstring1, instring) != NULL) {
				fprintf(stderr, "\n  Process Exists. Exiting ...\n\n");
				fclose(out);
				exit(0);
			}
			fclose(out);
		}
		sprintf(instring, "%s/%s_hf2_display_running", tmp_dir, prefix);
		remove(instring);
		fprintf(stderr, "\n  Process Does Not Exist. Lock File Removed\n");
	}

	sprintf(instring, "%s//%s_hf2_display_running", tmp_dir, prefix);
	fprintf(stderr, "\n%s", instring);
	out = fopen(instring, "w");
	if (out != NULL)
	  fprintf(out, "%i", getpid());
	else {
		fprintf(stderr, "ERROR!! %s", instring);
		exit(0);
	}
	fclose(out);

	/* try to restore latest values */

	read_level_files();
	fl_register_images();

	Fl::add_timeout(2.0, callback);
	Fl_Double_Window *win =
			new Fl_Double_Window(sx, sy, disp_name);
	win->begin();
	spec1 = new Ca_Canvas(10, round(sy*0.0482), round(sx*0.9), 512, "");
	spec1->box(FL_DOWN_BOX);
	spec1->align(FL_ALIGN_TOP);

	freq2 = new Ca_Y_Axis(round(sx*0.915), 30, 250, round(sy*0.619), "Freq.\n[kHz]");
	freq2->box(FL_FLAT_BOX);
	freq2->align(FL_ALIGN_TOP_RIGHT);
	freq2->axis_align(CA_RIGHT);
	freq2->scale(CA_LIN);
	freq2->minimum(freq_min);
	freq2->maximum(freq_max);
	//freq2->minimum(0);
	//freq2->maximum(5000);
	freq2->label_format("%g");
	freq2->tick_interval(-freq_step); //fixed ticks setting
	freq2->major_step(1);
	freq2->label_step(1);
	freq2->axis_color(FL_BLACK);

	canvas = new Ca_Canvas(10, round(sy*0.6766), round(sx*0.9), round(sy*0.2866), "Latest Spectra");
	canvas->box(FL_DOWN_BOX);
	canvas->align(FL_ALIGN_TOP);

	freq1 = new Ca_X_Axis(10, round(sy*0.963), round(sx*0.937), round(sy*0.0344), "Frequency [kHz]");
	freq1->align(FL_ALIGN_BOTTOM);
	freq1->scale(CA_LIN);
	freq1->minimum(10);
	freq1->maximum(5000);
	freq1->label_format("%g");
	freq1->major_step(2000);
	freq1->label_step(2000);
	freq1->axis_color(FL_BLACK);
	freq1->axis_align(CA_BOTTOM | CA_LINE);

	counts = new Ca_Y_Axis(round(sx*0.916), round(sy*0.665), round(sx*0.0419), round(sy*0.287), "log(counts)");
	counts->align(FL_ALIGN_TOP_LEFT);
	counts->minimum(0);
	counts->maximum(plot_max);
	counts->axis_align(CA_RIGHT);
	counts->tick_interval(-10); //fixed ticks setting
	counts->major_step(10);
	counts->label_step(1);
	counts->label_format("%.0f");

	savspec1_button = new Fl_Button(10, round(sy*0.6376), round(sx*0.1189), round(sy*0.02866), "Save spectrogram 1");
	savspec1_button->when(FL_WHEN_RELEASE);
	savspec1_button->callback(savspec_callback,&savspec1);

	// Grayscale #1 Channel Selector
	{
		int gs1selx = 10, gs1sely = 4;
		Fl_Group* o1 = new Fl_Group(gs1selx, gs1sely, round(sx*0.165), round(sy*0.0344));
		o1->box(FL_THIN_UP_FRAME);
		{
			Fl_Button* p = new Fl_Button(gs1selx + 2, gs1sely + 4, 110, 22, "GS1 Channel:");
			p->box(FL_FLAT_BOX);
			p->align(FL_ALIGN_INSIDE);
			p->labeltype(FL_NORMAL_LABEL);
			p->labelsize(16);
			p->deactivate();
		}
		{
			o1gray1 = new Fl_Round_Button(gs1selx + 110, gs1sely + 0, 20, 30, "&1");
			o1gray1->type(102);
			o1gray1->value(1);
			o1gray1->labelcolor(FL_YELLOW);
		}
		{
			o1gray2 = new Fl_Round_Button(gs1selx + 140, gs1sely + 0, 20, 30, "&2");
			o1gray2->type(102);
			o1gray2->value(0);
			o1gray2->labelcolor(FL_BLUE);
		}
		{
			o1gray3 = new Fl_Round_Button(gs1selx + 170, gs1sely + 0, 20, 30, "&3");
			o1gray3->type(102);
			o1gray3->value(0);
			o1gray3->labelcolor(FL_RED);
		}
		{
			o1gray4 = new Fl_Round_Button(gs1selx + 200, gs1sely + 0, 20, 30, "&4");
			o1gray4->type(102);
			o1gray4->value(0);
			o1gray4->labelcolor(FL_GREEN);
		}
		o1->end();
	}

	win->callback(WinQuit_CB, 0);
	cur_time = new Fl_Output(round(sx*0.6), 4, 208, 28, "Current Time");
	cur_time->align(FL_ALIGN_INSIDE);
	cur_time->value("Current Time");
	{
	  Fl_Return_Button* exit_button = new Fl_Return_Button(round(sx*.82), 4, 75, 30,
				"Exit");
		exit_button->color((Fl_Color) 23);
		exit_button->callback(WinQuit_CB, 0);
	}
/*	{
		Fl_Button* o = new Fl_Button(400, -1, 160, 30, "Dartmouth College");
		o->box(FL_FLAT_BOX);
		o->labeltype(FL_ENGRAVED_LABEL);
		o->labelsize(16);
		o->deactivate();
	}
	{
		Fl_Button* o = new Fl_Button(400, 20, 155, 30, "LF/MF/HF Receiver");
		o->box(FL_FLAT_BOX);
		o->labeltype(FL_ENGRAVED_LABEL);
		o->labelsize(16);
		o->deactivate();
	} */
	ds = new Flu_Dual_Slider(round(sx*0.804),round(sy*0.635), 130, 20, "");
	ds->type(FL_HOR_NICE_SLIDER);
	ds->high_value( plot_max / 100. );
	ds->low_value(plot_min / 100.);
	sprintf(outstring1, "Plot Range %d:%d",
			(int) ((100* ds ->low_value()) + .5), (int) ((100*
					ds ->high_value()) + .5));
	ds->label(outstring1);
	ds->callback(plot_level_callback, 0);

	ds2 = new Flu_Dual_Slider(450, 4, 180, 20, "");
	ds2->type(FL_HOR_NICE_SLIDER);
	ds2->high_value( ( gray_max ) / 100.);
	ds2->low_value(gray_min / 100.);
	sprintf(outstring2, "Grayscale Levels %d:%d", (int) ((100*
			ds2 ->low_value()) + .5), (int) ((100* ds2 ->high_value()) + .5));
	ds2->label(outstring2);
	ds2->callback(grayscale_level_callback, 0);

	win->end();
	win->show(argc, argv);
	read_input_file();
	return Fl::run();
}


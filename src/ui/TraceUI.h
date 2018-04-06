//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
    Fl_Slider*			m_threshSlider;
    Fl_Slider*			m_sampleNumSlider;
    Fl_Slider*          m_glossSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

    Fl_Light_Button*    m_jitterLightButton;
    Fl_Light_Button*    m_adaptLightButton;
    Fl_Light_Button*    m_rayDistLightButton;
	Fl_Light_Button*    m_textureLightButton;
	Fl_Light_Button*    m_backgroundLightButton;


	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
    int         getThresh();
    int         getSampleNum() { return m_sampleNum; }
    bool        isJitter() { return m_jitter; }
    bool        isAdaptive() { return m_adapt; }
    bool        isRayDist() { return m_rayDist; }
    double        getGloss() { return m_gloss; }

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
    double      m_nThresh = 0;
    int         m_sampleNum = 1;
    bool        m_jitter = FALSE;
    bool        m_adapt = FALSE;
    bool        m_rayDist = FALSE;
	bool		m_texture = FALSE;
	bool		m_background = FALSE;
    double      m_gloss = 0;


// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_texture(Fl_Menu_* o, void* v);
	static void cb_load_background(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
    static void cb_threshSlides(Fl_Widget* o, void* v);
    static void cb_sampleNumSlides(Fl_Widget* o, void* v);
    static void cb_glossSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
    static void cb_jitter(Fl_Widget* o, void* v);
    static void cb_adapt(Fl_Widget* o, void* v);
    static void cb_rayDist(Fl_Widget* o, void* v);
	static void cb_texture(Fl_Widget* o, void* v);
	static void cb_background(Fl_Widget* o, void* v);
};

#endif

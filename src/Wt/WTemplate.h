// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2009 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WTEMPLATE_H_
#define WTEMPLATE_H_

#include <Wt/WInteractWidget.h>
#include <Wt/WString.h>

namespace Wt {

class EscapeOStream;

/*! \brief Enumeration that indicates how a widget's ID may be set.
 *
 * \sa setWidgetIdMode()
 */
enum class TemplateWidgetIdMode {
  /*!
   * Do not set the widget ID.
   */
  None,

  /*!
   * Use setObjectName() to add a 'data-object-name' attribute. This is
   * a safe choice since it does not affect the ID.
   */
  SetObjectName,

  /*!
   * Use setId() to set the ID as the varName.
   *
   * \warning You must be careful that there are no two widgets with
   *          the same ID in yor application.
   */
  SetId
};

/*! \class WTemplate Wt/WTemplate.h Wt/WTemplate.h
 *  \brief A widget that renders an XHTML template.
 *
 * The XHTML template may contain references to variables which
 * replaced by strings are widgets.
 *
 * Since the template text may be supplied by a WString, you can
 * conveniently store the string in a message resource bundle, and
 * make it localized by using WString::tr().
 *
 * Placeholders (for variables and functions) are delimited by:
 * <tt>${...}</tt>. To use a literal <tt>"${"</tt>, use
 * <tt>"$${"</tt>.
 * Place holder names can contain '_', '-', '.' and alfanumeric characters.
 *
 * Usage example:
 * \if cpp
 * \code
 * WString userName = ...;
 *
 * auto t = std::make_unique<WTemplate>();
 * t->setTemplateText("<div> How old are you, ${friend} ? ${age-input} </div>");
 *
 * t->bindString("friend", userName, TextFormat::Plain);
 * ageEdit_ = t->bindWidget("age-input", std::make_unique<WLineEdit>());
 * \endcode
 * \elseif java
 * \code
 * WString userName = ...;
 *
 * WTemplate t = new WTemplate();
 * t.setTemplateText("<div> How old are you, ${friend} ? ${age-input} </div>");
 *
 * t.bindString("friend", userName, TextFormat::Plain);
 * t.bindWidget("age-input", ageEdit_ = new WLineEdit());
 * \endcode
 * \endif
 *
 * There are currently three syntactic constructs defined: variable
 * place holders, functions and conditional blocks.
 *
 * <h3>A. Variable placeholders</h3>
 *
 * <tt>${var}</tt> defines a placeholder for the variable "var", and
 * gets replaced with whatever is bound to that variable:
 * - a widget, using bindWidget()
 * - a string value, using bindString() or bindInt()
 * - or in general, the result of resolveString() and resolveWidget()
 *   methods.
 *
 * Optionally, additional arguments can be specified using the following
 * syntax:
 *
 * <tt>${var arg1="A value" arg2='A second value'}</tt>
 *
 * The arguments can thus be simple strings or quoted strings
 * (single or double quoted). These arguments are applied to a
 * resolved widget in applyArguments() and currently supports only
 * style classes.
 *
 * You can bind widgets and values to variables using bindWidget(),
 * bindString() or bindInt() or by reimplementing the resolveString()
 * and resolveWidget() methods.
 *
 * \note The use of XML comments (<tt>&lt;!\-- ... \--&gt;</tt>)
 *       around variables that are bound to widgets will result in bad
 *       behaviour since the template parser is ignorant about these
 *       comments and the corresponding widgets will believe that they
 *       are rendered but aren't actually.
 *
 * <h3>B. %Functions</h3>
 *
 * <tt>${fun:arg}</tt> defines a placeholder for applying a function
 * "fun" to an argument "arg".
 *
 * Optionally, additional arguments can be specified as with a
 * variable placeholder.
 *
 * Functions are resolved by resolveFunction(), and the default implementation
 * considers functions bound with addFunction().
 * There are currently three functions that are generally useful:
 * - \link Functions::tr\endlink : resolves a localized strings, this is
 *   convenient to create a language neutral template, which contains
 *   translated strings
 * - \link Functions::id\endlink : resolves the id of a bound widget, this
 *   is convenient to bind &lt;label&gt; elements to a form widget using its
 *   for attribute.
 * - \link Functions::block\endlink : recursively renders another string as
 *   macro block optional arguments substituted before processing template
 *   substitution.
 *
 * For example, the following template uses the "tr" function to translate
 * the age-label using the "age-label" internationalized key.
 *
 * \if cpp
 * \code
 * auto t = std::make_unique<WTemplate>();
 * t->addFunction("tr", &WTemplate::Functions::tr);
 * t->setTemplateText("<div> ${tr:age-label} ${age-input} </div>");
 * ageEdit_ = t->bindWidget("age-input", std::make_unique<WLineEdit>());
 * \endcode
 * \elseif java
 * \code
 * WTemplate t = new WTemplate();
 * t.setTemplateText("<div> ${tr:age-label} ${age-input} </div>");
 * t.addFunction("tr", WTemplate.Functions.tr);
 * t.bindWidget("age-input", ageEdit = new WLineEdit());
 * \endcode
 * \endif
 *
 * <h3>C. Conditional blocks</h3>
 *
 * <tt>${&lt;cond&gt;}</tt> starts a conditional block with a condition name
 * "cond", and must be closed by a balanced <tt>${&lt;/cond&gt;}</tt>.
 *
 * For example:
 * \if cpp
 * \code
 * auto t = std::make_unique<WTemplate>();
 * t->setTemplateText("<div> ${<if-register>} Register ... ${</if-register>}</div>");
 * t->setCondition("if-register", true);
 * \endcode
 * \elseif java
 * \code
 * WTemplate t = new WTemplate();
 * t.setTemplateText("<div> ${<if-register>} Register ... ${</if-register>}</div>");
 * t.setCondition("if-register", true);
 * \endcode
 * \endif
 *
 * Conditions are set using setCondition().
 *
 * \if cpp
 * The template can return a bound widget using resolve(), which already
 * tries to cast the widget to the proper type.
 * \endif
 *
 * <h3>CSS</h3>
 *
 * This widget does not provide styling,
 * and can be styled using inline or external CSS as appropriate.
 */
class WT_API WTemplate : public WInteractWidget
{
private:
  bool _tr(const std::vector<WString>& args,
           std::ostream& result);
  bool _block(const std::vector<WString>& args,
              std::ostream& result);
  bool _while(const std::vector<WString>& args,
              std::ostream& result);
  bool _id(const std::vector<WString>& args,
           std::ostream& result);

public:
  /*! \brief Typedef for enum Wt::TemplateWidgetIdMode */
  typedef TemplateWidgetIdMode WidgetIdMode;

#ifndef WT_TARGET_JAVA
  /*! \brief A function type
   *
   * \sa addFunction()
   * \sa Functions::tr, Functions::id, Functions::block, Functions::while_f
   */
  typedef std::function<bool(WTemplate *t, const std::vector<WString>& args,
                             std::ostream& result)> Function;

#else
  /*! \brief A function interface type
   *
   * \sa addFunction()
   * \sa Functions::tr, Functions::id, Functions::block, Functions::while_f
   */
  class Function {
  public:
    virtual bool evaluate(WTemplate *t, const std::vector<WString>& args,
                          std::ostream& result) const = 0;

  };

private:
  class TrFunction : public Function {
  public:
    virtual bool evaluate(WTemplate *t, const std::vector<WString>& args,
                          std::ostream& result) const;
  };

  class BlockFunction : public Function {
  public:
    virtual bool evaluate(WTemplate *t, const std::vector<WString>& args,
                          std::ostream& result) const;
  };

  class WhileFunction : public Function {
  public:
    virtual bool evaluate(WTemplate *t, const std::vector<WString>& args,
                          std::ostream& result) const;
  };

  class IdFunction : public Function {
  public:
    virtual bool evaluate(WTemplate *t, const std::vector<WString>& args,
                          std::ostream& result) const;
  };
public:
#endif
  /*! \brief A collection of predefined functions
   *
   * \sa addFunction()
   */
  struct WT_API Functions
  {
    /*! \brief A function that resolves to a localized string.
     *
     * For example, when bound to the function <tt>"tr"</tt>, template
     * that contains the placeholder
     * \code
     *   ... ${tr:name} ...
     * \endcode
     * will be resolved to the value of:
     * \code
     *   WString::tr("name")
     * \endcode
     *
     * \sa addFunction()
     */
#ifndef WT_TARGET_JAVA
    static bool tr(WTemplate *t, const std::vector<WString>& args,
                   std::ostream& result);
#else
    static Function& tr = TrFunction();
#endif

    /*! \brief A function that renders a macro block.
     *
     * The function will consider the first argument as the key
     * for a localized string that is a macro block, and additional arguments
     * as positional parameters in that block.
     *
     * For example, a template that contains:
     * \code
     * ...
     * ${block:form-field category}
     * ...
     * \endcode
     *
     * would look-up the following message:
     *
     * \code
     * <message id="form-field">
     *    <div class="control-group">
     *       ${{1}-info}
     *    </div>
     * </message>
     * \endcode
     *
     * and render as:
     *
     * \code
     * ...
     * <div class="control-group">
     *   ${category-info}
     * </div>
     * ...
     * \endcode
     */
#ifndef WT_TARGET_JAVA
    static bool block(WTemplate *t, const std::vector<WString>& args,
                      std::ostream& result);
#else
    static Function& block = BlockFunction();
#endif

    /*! \brief A function that renders a macro block as long as the given condition is true
     *
     * The function will consider the first argument as the condition, and the
     * second argument as the key for a localized string that is a macro block.
     *
     * Just like the block() function, you can provide additional arguments, so the third
     * argument will be what is filled in into <tt>{1}</tt> in the macro block, etc.
     */
#ifndef WT_TARGET_JAVA
    static bool while_f(WTemplate *t, const std::vector<WString>& args,
                      std::ostream& result);
#else
    static Function& while_f = WhileFunction();
#endif

    /*! \brief A function that resolves the id of a bound widget
     *
     * For example, when bound to the function <tt>"id"</tt>, template text
     * that contains a place-holder
     * \code
     *   ... ${id:name} ...
     * \endcode
     *
     * will be resolved to the value of:
     * \if cpp
     * \code
     *   t->resolveWidget("name")->id()
     * \endcode
     * \elseif java
     * \code
     *   t.resolveWidget("name").id()
     * \endcode
     * \endif
     *
     * This is useful for binding labels to input elements.
     *
     * \sa addFunction()
     */
#ifndef WT_TARGET_JAVA
    static bool id(WTemplate *t, const std::vector<WString>& args,
                   std::ostream& result);
#else
    static Function& id = IdFunction();
#endif // WT_TARGET_JAVA
  };

  /*! \brief Creates a template widget.
   */
  WTemplate();

  /*! \brief Creates a template widget with given template.
   *
   * The \p templateText must be proper XHTML, and this is checked
   * unless the XHTML is resolved from a message resource bundle. This
   * behavior is similar to a WText when configured with the
   * Wt::TextFormat::XHTML textformat.
   */
  WTemplate(const WString& text);

  virtual ~WTemplate();

  /*! \brief Returns the template.
   *
   * \sa setTemplateText()
   */
  virtual WString templateText() const { return text_; }

  /*! \brief Sets the template text.
   *
   * The \p text must be proper XHTML, and this is checked unless the
   * XHTML is resolved from a message resource bundle or TextFormat is
   * Wt::TextFormat::UnsafeXHTML. This behavior is similar to a WText when
   * configured with the Wt::TextFormat::XHTML textformat.
   *
   * Changing the template text does not clear() bound widgets or
   * values.
   *
   * \sa clear()
   */
  void setTemplateText(const WString& text,
                       TextFormat textFormat = TextFormat::XHTML);

  /*! \brief Sets how the varName should be reflected on bound widgets.
   *
   * To easily identify a widget in the browser, it may be convenient
   * to reflect the varName, either through the object name (recommended)
   * or the widget's ID.
   *
   * The default value is TemplateWidgetIdMode::None which does not reflect the
   * varName on the bound widget.
   */
  void setWidgetIdMode(TemplateWidgetIdMode mode);

  /*! \brief Returns how the varName is reflected on a bound widget.
   *
   * \sa setWidgetIdMode()
   */
  TemplateWidgetIdMode widgetIdMode() const { return widgetIdMode_; }

  /*! \brief Binds a string value to a variable.
   *
   * Each occurrence of the variable within the template will be
   * substituted by its value.
   *
   * \note Depending on the \p textFormat, the \p value is validated according
   * as for a WText. The default (TextFormat::XHTML) filters "active" content, to avoid
   * XSS-based security risks.
   *
   * \sa bindWidget(), bindInt()
   * \sa resolveString()
   */
  virtual void bindString(const std::string& varName, const WString& value,
                          TextFormat textFormat = TextFormat::XHTML);

  /*! \brief Binds an integer value to a variable.
   *
   * \sa bindString()
   */
  void bindInt(const std::string& varName, int value);

  /*! \brief Binds a widget to a variable.
   *
   * The corresponding variable reference within the template will be
   * replaced with the widget (rendered as XHTML). Since a single
   * widget may be instantiated only once in a template, the variable
   * \p varName may occur at most once in the template, and the
   * \p widget must not yet be bound to another variable.
   *
   * The widget is reparented to the WTemplate, so that it is deleted
   * when the WTemplate is deleted.
   *
   * If a widget was already bound to the variable, it is deleted
   * first. If previously a string or other value was bound to the
   * variable, it is removed.
   *
   * You may also pass a \c nullptr \p widget, which will resolve to an empty
   * string.
   *
   * \sa bindString()
   * \sa resolveWidget()
   */
  virtual
#ifndef WT_TARGET_JAVA
  void
#else // WT_TARGET_JAVA
  WWidget*
#endif // WT_TARGET_JAVA
  bindWidget(const std::string& varName,
                          std::unique_ptr<WWidget> widget);

  /*! \brief Binds a widget to a variable, returning a raw pointer.
   *
   * This is implemented as:
   *
   * \code
   * Widget *result = widget.get();
   * bindWidget(varName, std::unique_ptr<WWidget>(std::move(widget)));
   * return result;
   * \endcode
   */
  template <typename Widget>
    Widget *bindWidget(const std::string& varName,
                       std::unique_ptr<Widget> widget)
#ifndef WT_TARGET_JAVA
  {
    Widget *result = widget.get();
    bindWidget(varName, std::unique_ptr<WWidget>(std::move(widget)));
    return result;
  }
#else // WT_TARGET_JAVA
  ;
#endif // WT_TARGET_JAVA

#ifndef WT_TARGET_JAVA
  /*! \brief Creates a new widget with the given arguments, and binds it, returning a raw pointer.
   *
   * This is implemented as:
   *
   * \code
   * std::unique_ptr<Widget> w{new Widget(std::forward<Args>(args)...)};
   * Widget *result = w.get();
   * bindWidget(varName, std::unique_ptr<WWidget>(std::move(w)));
   * return result;
   * \endcode
   *
   * This is a useful shorthand for creating and binding a widget in one go.
   */
  template <typename Widget, typename ...Args>
    Widget *bindNew(const std::string& varName,
                    Args&& ...args)
  {
    std::unique_ptr<Widget> w{new Widget(std::forward<Args>(args)...)};
    Widget *result = w.get();
    bindWidget(varName, std::unique_ptr<WWidget>(std::move(w)));
    return result;
  }
#else // WT_TARGET_JAVA
  template <typename Widget>
    Widget *bindNew(const std::string& varName);
  template <typename Widget, typename Arg1>
    Widget *bindNew(const std::string& varName, Arg1 arg1);
  template <typename Widget, typename Arg1, typename Arg2>
    Widget *bindNew(const std::string& varName, Arg1 arg1, Arg2 arg2);
  template <typename Widget, typename Arg1, typename Arg2, typename Arg3>
    Widget *bindNew(const std::string& varName, Arg1 arg1, Arg2 arg2, Arg3 arg3);
  template <typename Widget, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    Widget *bindNew(const std::string& varName, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);
#endif // WT_TARGET_JAVA

  using WWidget::removeWidget;

  /*! \brief Unbinds a widget by variable name.
   *
   * This removes a previously bound widget and unbinds the
   * corresponding variable, effectively undoing the effect of
   * bindWidget().
   *
   * If this template does not contain a widget for the given
   * \p varName, \c nullptr is returned.
   */
  std::unique_ptr<WWidget> removeWidget(const std::string& varName);

  /*! \brief Unbinds a widget by widget pointer.
   *
   * This removes a previously bound widget and unbinds the
   * corresponding variable, effectively undoing the effect of
   * bindWidget().
   *
   * If this template does not contain the given widget,
   * \c nullptr is returned.
   */
  virtual std::unique_ptr<WWidget> removeWidget(WWidget *widget) override;

  /*! \brief Binds an empty string to a variable.
   *
   * If a widget was bound to the variable, it is deleted first.
   *
   * \sa bindString()
   */
  void bindEmpty(const std::string& varName);

  /*! \brief Binds a function.
   *
   * Functions are useful to automatically resolve placeholders.
   *
   * The syntax for a function 'fun' applied to a single argument
   * 'bla' is:
   *
   * <tt>${fun:bla}</tt>
   *
   * There are three predefined functions, which can be bound using:
   * \if cpp
   * \code
   * WTemplate *t = ...;
   * t->addFunction("id", &WTemplate::Functions::id);
   * t->addFunction("tr", &WTemplate::Functions::tr);
   * t->addFunction("block", &WTemplate::Functions::block);
   * \endcode
   * \else
   * \code
   * WTemplate t = ...;
   * t.addFunction("id", WTemplate.Functions.id);
   * t.addFunction("tr", WTemplate.Functions.tr);
   * t.addFunction("block", WTemplate.Functions.block);
   * \endcode
   * \endif
   */
#ifndef WT_TARGET_JAVA
  void addFunction(const std::string& name, const Function& function);
#else
  void addFunction(const std::string& name, const Function *function);
#endif

  /*! \brief Sets a condition.
   *
   * This enables or disables the inclusion of a conditional block.
   *
   * The default value of all conditions is \c false.
   */
  void setCondition(const std::string& name, bool value);

  /*! \brief Returns a condition value.
   *
   * \sa setCondition()
   */
  virtual bool conditionValue(const std::string& name) const;

  /*! \brief Returns the set of conditions set to true.
   */
  std::set<std::string> conditionsSet() { return conditions_; }

  /*! \brief Resolves the string value for a variable name.
   *
   * This is the main method used to resolve variables in the template
   * text, during rendering.
   *
   * The default implementation considers first whether a string was
   * bound using bindString(). If so, that string is returned. If
   * not, it will attempt to resolve a widget with that variable name
   * using resolveWidget(), and render it as XHTML. If that fails too,
   * handleUnresolvedVariable() is called, passing the initial arguments.
   *
   * You may want to reimplement this method to provide on-demand
   * loading of strings for your template.
   *
   * The result stream expects a UTF-8 encoded string value.
   *
   * \warning When specializing this class, you need to make sure that
   * you append proper XHTML to the \p result, without unsafe active
   * contents. The format() methods may be used for this purpose.
   *
   * \sa renderTemplate()
   */
  virtual void resolveString(const std::string& varName,
                             const std::vector<WString>& args,
                             std::ostream& result);

  /*! \brief Handles a variable that could not be resolved.
   *
   * This method is called from resolveString() for variables that could
   * not be resolved.
   *
   * The default implementation implementation writes
   * "??" + varName + "??"  to the result stream.
   *
   * The result stream expects a UTF-8 encoded string value.
   *
   * \warning When specializing this class, you need to make sure that
   * you append proper XHTML to the \p result, without unsafe active
   * contents. The format() methods may be used for this purpose.
   *
   * \sa resolveString()
   */
   virtual void handleUnresolvedVariable(const std::string& varName,
                                         const std::vector<WString>& args,
                                         std::ostream& result);

  /*! \brief Resolves a widget for a variable name.
   *
   * The default implementation returns a widget that was bound using
   * bindWidget().
   *
   * You may want to reimplement this method to create widgets
   * on-demand. All widgets that are returned by this method are
   * reparented to the WTemplate, so they will be deleted when the
   * template is destroyed, but they are not deleted by clear() (unless
   * bind was called on them as in the example below).
   *
   * This method is typically used for delayed binding of widgets.
   * Usage example:
   * \if cpp
   * \code
   * if (Wt::WWidget *known = WTemplate::resolveWidget(varName)) {
   *   return known;
   * } else {
   *   if (varName == "age-input") {
   *     // widget only created when used
   *     return bindWidget(varName, std::make_unique<Wt::WLineEdit>());
   *   }
   *   return nullptr;
   * }
   * \endcode
   * \elseif java
   * \code
   * if (WWidget known = super.resolveWidget(varName)) {
   *   return known;
   * } else {
   *   if (varName == "age-input") {
   *     WWidget w = new WLineEdit(); // widget only created when used
   *     bindWidget(varName, w);
   *     return w;
   *   }
   * }
   * \endcode
   * \endif
   */
  virtual WWidget *resolveWidget(const std::string& varName);

  std::vector<WWidget *> widgets() const;
  std::string varName(WWidget *w) const;

  /*! \brief Resolves a function call.
   *
   * This resolves a function with name \p name, and one or more arguments
   * \p args, and writes the result into the stream \p result. The method
   * returns whether a function was matched and applied.
   *
   * The default implementation considers functions that were bound
   * using addFunction().
   *
   * \sa addFunction()
   */
  virtual bool resolveFunction(const std::string& name,
                               const std::vector<WString>& args,
                               std::ostream& result);

  /*! \brief Returns a widget for a variable name.
   *
   * This is a convience method, which calls resolveWidget() and dynamic casts
   * the result to type \p T. You may use this method to fetch widgets
   * that have previously been bound using bindWidget().
   *
   * If the cast fails, a null pointer is returned.
   */
  template <typename T> T resolve(const std::string& varName);

  /*! \brief Erases all variable bindings.
   *
   * Removes all strings and deletes all widgets that were previously
   * bound using bindString() and bindWidget().
   *
   * This also resets all conditions set using setCondition(), but
   * does not remove functions added with addFunction()
   */
  virtual void clear();

  /*! \brief Enables internal path anchors in the XHTML template.
   *
   * Anchors to internal paths are represented differently depending
   * on the session implementation (plain HTML, Ajax or HTML5
   * history). By enabling this option, anchors which reference an
   * internal path (by referring a URL of the form
   * <tt>href="#/..."</tt>), are re-encoded to link to the internal
   * path.
   *
   * The default value is \c false.
   *
   * \sa WAnchor::setRefInternalPath()
   */
  void setInternalPathEncoding(bool enabled);

  /*! \brief Returns whether internal paths are enabled.
   *
   * \sa setInternalPathEncoding()
   */
  bool hasInternalPathEncoding() const { return encodeInternalPaths_; }

  /*! \brief Configures when internal path encoding is done.
   *
   * By default, the internal path encoding (if enabled) is done on
   * the template text before placeholders are being resolved. In some
   * rare situations, you may want to postpone the internal path
   * encoding until after placeholders have been resolved, e.g. if a
   * placeholder was used to provide the string for an anchor href.
   *
   * The default value is \c true
   */
  void setEncodeTemplateText(bool on);

  /*! \brief Returns whether internal path encoding is done on the template text.
   * \sa setEncodeTemplateText()
   */
  bool encodeTemplateText() const { return encodeTemplateText_; }

  virtual void refresh() override;

  /*! \brief Renders the template into the given result stream.
   *
   * The default implementation will call renderTemplateText() with the
   * templateText().
   */
  virtual void renderTemplate(std::ostream& result);

  /*! \brief Renders a template into the given result stream.
   *
   * The default implementation will parse the template, and resolve variables
   * by calling resolveString().
   *
   * You may want to reimplement this method to manage resources that are
   * needed to load content on-demand (e.g. database objects), or support
   * a custom template language.
   *
   * Return: true if rendered successfully.
   * \sa getErrorText()
   */
  bool renderTemplateText(std::ostream& result, const WString& templateText);

  /*! \brief Renders the errors during renderring.
   *  \sa renderTemplateText()
   */
  std::string getErrorText() {return errorText_;}

protected:
  /*! \brief Applies arguments to a resolved widget.
   *
   * Currently only a <tt>class</tt> argument is handled, which adds
   * one or more style classes to the widget \p w, using
   * WWidget::addStyleClass().
   */
  virtual void applyArguments(WWidget *w, const std::vector<WString>& args);

  virtual void updateDom(DomElement& element, bool all) override;
  virtual DomElementType domElementType() const override;
  virtual void propagateRenderOk(bool deep) override;
  virtual void iterateChildren(const HandleWidgetMethod& method) const override;

  /*! \brief Utility method to safely format an XHTML string.
   *
   * The string is formatted according to the indicated \p
   * textFormat. It is recommended to use this method when
   * specializing resolveString() to avoid security risks.
   */
  void format(std::ostream& result, const std::string& s,
              TextFormat textFormat = TextFormat::Plain);

  /*! \brief Utility method to safely format an XHTML string.
   *
   * The string is formatted according to the indicated \p
   * textFormat. It is recommended to use this method when
   * specializing resolveString() to avoid security risks.
   */
  void format(std::ostream& result, const WString& s,
              TextFormat textFormat = TextFormat::Plain);

  virtual void enableAjax() override;

  /*! \brief Notifies the template that it has changed and must be rerendered.
   *
   * If you update a WTemplate with e.g bindWidget or setCondition,
   * or change the template text, the template will automatically be
   * rerendered.
   *
   * However, if you create a subclass of WTemplate and override resolveString or
   * resolveWidget, you will have to notify the WTemplate if it has changed with
   * a call to reset().
   */
  void reset();

private:
  typedef std::map<std::string, Function> FunctionMap;
  typedef std::map<std::string, WString> StringMap;
  typedef std::map<std::string, std::unique_ptr<WWidget> > WidgetMap;
  typedef std::set<std::string> ConditionSet;

  std::set<WWidget *> *previouslyRendered_;
  std::vector<WWidget *> *newlyRendered_;

  FunctionMap functions_;
  StringMap strings_;
  WidgetMap widgets_;
  ConditionSet conditions_;

  WString text_;
  std::string errorText_;

  bool encodeInternalPaths_, encodeTemplateText_, changed_;
  TemplateWidgetIdMode widgetIdMode_;

  std::string encode(const std::string& text) const;
  static std::size_t parseArgs(const std::string& text,
                               std::size_t pos,
                               std::vector<WString>& result);
  void unrenderWidget(WWidget *w, DomElement &el);

  EscapeOStream* plainTextNewLineEscStream_;
};

template <typename T> T WTemplate::resolve(const std::string& varName)
{
  WWidget *w = resolveWidget(varName);
  return dynamic_cast<T>(w);
}


}

#endif // WTEMPLATE_H_

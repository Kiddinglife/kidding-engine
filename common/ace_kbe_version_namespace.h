#ifndef ACE_KBE_VERSIONED_NAMESPACE_H_
#define ACE_KBE_VERSIONED_NAMESPACE_H_

//#ifndef CONFIG_H_
//# error This header is only meant to be included by or after "ace/config-lite.h".
//#endif  /* !ACE_CONFIG_LITE_H */

//defined in custom.h
#if defined (ACE_KBE_HAS_VERSIONED_NAMESPACE) && ACE_KBE_HAS_VERSIONED_NAMESPACE == 1 
# ifndef ACE_KBE_VERSIONED_NAMESPACE_NAME
// Preprocessor symbols will not be expanded if they are
// concatenated.  Force the preprocessor to expand them during the
// argument prescan by calling a macro that itself calls another that
// performs the actual concatenation.
#  define ACE_KBE_MAKE_VERSIONED_NAMESPACE_NAME_IMPL(MAJOR,MINOR,BETA) KBE_ ## MAJOR ## _ ## MINOR ## _ ## BETA
#  define ACE_KBE_MAKE_VERSIONED_NAMESPACE_NAME(MAJOR,MINOR,BETA) ACE_KBE_MAKE_VERSIONED_NAMESPACE_NAME_IMPL(MAJOR,MINOR,BETA)
#  define ACE_KBE_VERSIONED_NAMESPACE_NAME ACE_KBE_MAKE_VERSIONED_NAMESPACE_NAME(0,0,1)
# endif  /* !ACE_VERSIONED_NAMESPACE_NAME */

#define ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL namespace ACE_KBE_VERSIONED_NAMESPACE_NAME{ 

#define ACE_KBE_END_VERSIONED_NAMESPACE_DECL  }; using namespace ACE_KBE_VERSIONED_NAMESPACE_NAME; 

#else

# define ACE_KBE_VERSIONED_NAMESPACE_NAME
# define ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
# define ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif  /* KBEHAS_VERSIONED_NAMESPACE */

#define NETWORK_NAMESPACE_BEGIN_DECL namespace NETWORK {
#define NETWORK_NAMESPACE_END_DECL }; using namespace NETWORK;
#endif  /* !KBE_VERSIONED_NAMESPACE_H */

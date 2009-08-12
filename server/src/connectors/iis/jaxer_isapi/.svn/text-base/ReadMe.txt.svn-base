================================================================================
       jaxer_isapi -- Jaxer ISAPI filter & extension for IIS 5 & 5.1
================================================================================

How to configure:
    Register the ISAPI as a global filter.
    
    For the Virtual Directory (supports only one) that Jaxer will process
    the request, Configure jaxer_isapi.dll with extension .jaxer for ALL vers.
    
    Edit jaxer_isapi.conf that is in the same directory as jaxer_isapi.dll.
    Read the descriptions carefully when modifying the conf file.
    

Limitations:
    jaxer_isapi must be registered as a global filter in order to get postdata.
    However, the filter may not be able to get the postdata if a) the postdata 
    is large and the extension that handles the request does not read the
    postdata, or b) the request is SSL.  When jaxer_isapi cannot get the entire
    postdata, postdata will not be available to jaxer.
    
    There is no rotation for the log file.  You will have to delete it if it 
    grows too large.
    
    Only one VirtualDirectory can be configured to use Jaxer.
    
    
     

<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	version="1.0">

<xsl:output method="xml" indent="yes"/>

<xsl:template match="testsuite">
    <xsl:copy>
        <xsl:apply-templates select="@*"/>
        <xsl:apply-templates select="//table"/>
    </xsl:copy>
</xsl:template>

<xsl:template match="table">
	<xsl:choose>
		<xsl:when test="contains(tbody/tr[1]/@class, 'status_passed')">
			<testcase>
				<xsl:attribute name="name">
					<xsl:value-of select="./tbody/tr[1]/td[1]"/>
				</xsl:attribute>
			</testcase>
		</xsl:when>
		<xsl:when test="contains(tbody/tr[1]/@class, 'status_failed')">
			<testcase>
				<xsl:attribute name="name">
					<xsl:value-of select="tbody/tr[1]/td[1]"/>
				</xsl:attribute>
				<failure>
					<xsl:value-of select="tbody/tr[contains(@class, 'status_failed')]/td[3]"/>
				</failure>
			</testcase>
		</xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template match="@*">
	<xsl:copy/>
</xsl:template>

</xsl:stylesheet>
